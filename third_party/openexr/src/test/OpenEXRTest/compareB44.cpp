//
// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) Contributors to the OpenEXR Project.
//

#ifdef NDEBUG
#    undef NDEBUG
#endif

#include "compareB44.h"

#include <algorithm>
#include <cassert>

using namespace OPENEXR_IMF_NAMESPACE;
using namespace std;

int
shiftAndRound (int x, int shift)
{
    x <<= 1;
    int a = (1 << shift) - 1;
    shift += 1;
    int b = (x >> shift) & 1;
    return (x + a + b) >> shift;
}

bool
withinB44ErrorBounds (const half A[4][4], const half B[4][4])
{
    //
    // Assuming that a 4x4 pixel block, B, was generated by
    // compressing and uncompressing another pixel block, A,
    // using OpenEXR's B44 compression method, check whether
    // the differences between A and B are what we would
    // expect from the compressor.
    //

    //
    // The block may not have been compressed at all if it
    // was part of a very small tile.
    //

    bool equal = true;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (A[i][j] != B[i][j]) equal = false;

    if (equal) return true;

    //
    // The block was compressed.
    //
    // Perform a "light" version of the B44 compression on A
    // (see the pack() function in ImfB44Compressor.cpp).
    //

    unsigned short t[16];

    for (int i = 0; i < 16; ++i)
    {
        unsigned short Abits = A[i / 4][i % 4].bits ();

        if ((Abits & 0x7c00) == 0x7c00)
            t[i] = 0x8000;
        else if (Abits & 0x8000)
            t[i] = ~Abits;
        else
            t[i] = Abits | 0x8000;
    }

    unsigned short tMax = 0;

    for (int i = 0; i < 16; ++i)
        if (tMax < t[i]) tMax = t[i];

    int shift = -1;
    int d[16];
    int r[15];
    int rMin;
    int rMax;

    do
    {
        shift += 1;

        for (int i = 0; i < 16; ++i)
            d[i] = shiftAndRound (tMax - t[i], shift);

        const int bias = 0x20;

        r[0] = d[0] - d[4] + bias;
        r[1] = d[4] - d[8] + bias;
        r[2] = d[8] - d[12] + bias;

        r[3] = d[0] - d[1] + bias;
        r[4] = d[4] - d[5] + bias;
        r[5] = d[8] - d[9] + bias;
        r[6] = d[12] - d[13] + bias;

        r[7]  = d[1] - d[2] + bias;
        r[8]  = d[5] - d[6] + bias;
        r[9]  = d[9] - d[10] + bias;
        r[10] = d[13] - d[14] + bias;

        r[11] = d[2] - d[3] + bias;
        r[12] = d[6] - d[7] + bias;
        r[13] = d[10] - d[11] + bias;
        r[14] = d[14] - d[15] + bias;

        rMin = r[0];
        rMax = r[0];

        for (int i = 1; i < 15; ++i)
        {
            if (rMin > r[i]) rMin = r[i];

            if (rMax < r[i]) rMax = r[i];
        }
    } while (rMin < 0 || rMax > 0x3f);

    t[0] = tMax - (d[0] << shift);

    //
    // Now perform a "light" version of the decompression method.
    // (see the unpack() function in ImfB44Compressor.cpp).
    //

    unsigned short A1[16];
    const int      bias = 0x20 << shift;

    A1[0]  = t[0];
    A1[4]  = A1[0] + (r[0] << shift) - bias;
    A1[8]  = A1[4] + (r[1] << shift) - bias;
    A1[12] = A1[8] + (r[2] << shift) - bias;

    A1[1]  = A1[0] + (r[3] << shift) - bias;
    A1[5]  = A1[4] + (r[4] << shift) - bias;
    A1[9]  = A1[8] + (r[5] << shift) - bias;
    A1[13] = A1[12] + (r[6] << shift) - bias;

    A1[2]  = A1[1] + (r[7] << shift) - bias;
    A1[6]  = A1[5] + (r[8] << shift) - bias;
    A1[10] = A1[9] + (r[9] << shift) - bias;
    A1[14] = A1[13] + (r[10] << shift) - bias;

    A1[3]  = A1[2] + (r[11] << shift) - bias;
    A1[7]  = A1[6] + (r[12] << shift) - bias;
    A1[11] = A1[10] + (r[13] << shift) - bias;
    A1[15] = A1[14] + (r[14] << shift) - bias;

    //
    // Compare the result with B, allowing for an difference
    // of a couple of units in the last place.
    //

    for (int i = 0; i < 16; ++i)
    {
        unsigned short A1bits = A1[i];
        unsigned short Bbits  = B[i / 4][i % 4].bits ();

        if (Bbits & 0x8000)
            Bbits = ~Bbits;
        else
            Bbits = Bbits | 0x8000;

        if (Bbits > A1bits + 5 || Bbits < A1bits - 5) return false;
    }

    return true;
}

void
compareB44 (
    int width, int height, const Array2D<half>& p1, const Array2D<half>& p2)
{
    for (int y = 0; y < height; y += 4)
    {
        for (int x = 0; x < width; x += 4)
        {
            half A[4][4];
            half B[4][4];

            for (int y1 = 0; y1 < 4; ++y1)
            {
                for (int x1 = 0; x1 < 4; ++x1)
                {
                    int y2    = min (y + y1, height - 1);
                    int x2    = min (x + x1, width - 1);
                    A[y1][x1] = p1[y2][x2];
                    B[y1][x1] = p2[y2][x2];
                }
            }

            assert (withinB44ErrorBounds (A, B));
        }
    }
}

void
compareB44 (
    int                  width,
    int                  height,
    const Array2D<Rgba>& p1,
    const Array2D<Rgba>& p2,
    RgbaChannels         channels)
{
    if (channels & WRITE_R)
    {
        for (int y = 0; y < height; y += 4)
        {
            for (int x = 0; x < width; x += 4)
            {
                half A[4][4];
                half B[4][4];

                for (int y1 = 0; y1 < 4; ++y1)
                {
                    for (int x1 = 0; x1 < 4; ++x1)
                    {
                        int y2    = min (y + y1, height - 1);
                        int x2    = min (x + x1, width - 1);
                        A[y1][x1] = p1[y2][x2].r;
                        B[y1][x1] = p2[y2][x2].r;
                    }
                }

                assert (withinB44ErrorBounds (A, B));
            }
        }
    }
    else
    {
        for (int y = 0; y < height; y += 1)
            for (int x = 0; x < width; x += 1)
                assert (p2[y][x].r == 0);
    }

    if (channels & WRITE_G)
    {
        for (int y = 0; y < height; y += 4)
        {
            for (int x = 0; x < width; x += 4)
            {
                half A[4][4];
                half B[4][4];

                for (int y1 = 0; y1 < 4; ++y1)
                {
                    for (int x1 = 0; x1 < 4; ++x1)
                    {
                        int y2    = min (y + y1, height - 1);
                        int x2    = min (x + x1, width - 1);
                        A[y1][x1] = p1[y2][x2].g;
                        B[y1][x1] = p2[y2][x2].g;
                    }
                }

                assert (withinB44ErrorBounds (A, B));
            }
        }
    }
    else
    {
        for (int y = 0; y < height; y += 1)
            for (int x = 0; x < width; x += 1)
                assert (p2[y][x].g == 0);
    }

    if (channels & WRITE_B)
    {
        for (int y = 0; y < height; y += 4)
        {
            for (int x = 0; x < width; x += 4)
            {
                half A[4][4];
                half B[4][4];

                for (int y1 = 0; y1 < 4; ++y1)
                {
                    for (int x1 = 0; x1 < 4; ++x1)
                    {
                        int y2    = min (y + y1, height - 1);
                        int x2    = min (x + x1, width - 1);
                        A[y1][x1] = p1[y2][x2].b;
                        B[y1][x1] = p2[y2][x2].b;
                    }
                }

                assert (withinB44ErrorBounds (A, B));
            }
        }
    }
    else
    {
        for (int y = 0; y < height; y += 1)
            for (int x = 0; x < width; x += 1)
                assert (p2[y][x].b == 0);
    }

    if (channels & WRITE_A)
    {
        for (int y = 0; y < height; y += 4)
        {
            for (int x = 0; x < width; x += 4)
            {
                half A[4][4];
                half B[4][4];

                for (int y1 = 0; y1 < 4; ++y1)
                {
                    for (int x1 = 0; x1 < 4; ++x1)
                    {
                        int y2    = min (y + y1, height - 1);
                        int x2    = min (x + x1, width - 1);
                        A[y1][x1] = p1[y2][x2].a;
                        B[y1][x1] = p2[y2][x2].a;
                    }
                }

                assert (withinB44ErrorBounds (A, B));
            }
        }
    }
    else
    {
        for (int y = 0; y < height; y += 1)
            for (int x = 0; x < width; x += 1)
                assert (p2[y][x].a == 1);
    }
}
