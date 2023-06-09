//
// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) Contributors to the OpenEXR Project.
//

#ifndef INCLUDED_IMF_CHANNEL_LIST_H
#define INCLUDED_IMF_CHANNEL_LIST_H

//-----------------------------------------------------------------------------
//
//	class Channel
//	class ChannelList
//
//-----------------------------------------------------------------------------

#include "ImfForward.h"

#include "ImfName.h"
#include "ImfPixelType.h"

#include <map>
#include <set>
#include <string>

OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_ENTER

struct IMF_EXPORT_TYPE Channel
{
    //------------------------------
    // Data type; see ImfPixelType.h
    //------------------------------

    PixelType type;

    //--------------------------------------------
    // Subsampling: pixel (x, y) is present in the
    // channel only if
    //
    //  x % xSampling == 0 && y % ySampling == 0
    //
    //--------------------------------------------

    int xSampling;
    int ySampling;

    //--------------------------------------------------------------
    // Hint to lossy compression methods that indicates whether
    // human perception of the quantity represented by this channel
    // is closer to linear or closer to logarithmic.  Compression
    // methods may optimize image quality by adjusting pixel data
    // quantization according to this hint.
    // For example, perception of red, green, blue and luminance is
    // approximately logarithmic; the difference between 0.1 and 0.2
    // is perceived to be roughly the same as the difference between
    // 1.0 and 2.0.  Perception of chroma coordinates tends to be
    // closer to linear than logarithmic; the difference between 0.1
    // and 0.2 is perceived to be roughly the same as the difference
    // between 1.0 and 1.1.
    //--------------------------------------------------------------

    bool pLinear;

    //------------
    // Constructor
    //------------

    IMF_EXPORT
    Channel (
        PixelType type      = HALF,
        int       xSampling = 1,
        int       ySampling = 1,
        bool      pLinear   = false);

    //------------
    // Operator ==
    //------------

    IMF_EXPORT
    bool operator== (const Channel& other) const;
};

class IMF_EXPORT_TYPE ChannelList
{
public:
    //--------------
    // Add a channel
    //--------------

    IMF_EXPORT
    void insert (const char name[], const Channel& channel);

    IMF_EXPORT
    void insert (const std::string& name, const Channel& channel);

    //------------------------------------------------------------------
    // Access to existing channels:
    //
    // [n]		Returns a reference to the channel with name n.
    //			If no channel with name n exists, an IEX_NAMESPACE::ArgExc
    //			is thrown.
    //
    // findChannel(n)	Returns a pointer to the channel with name n,
    //			or 0 if no channel with name n exists.
    //
    //------------------------------------------------------------------

    IMF_EXPORT
    Channel& operator[] (const char name[]);
    IMF_EXPORT
    const Channel& operator[] (const char name[]) const;

    IMF_EXPORT
    Channel& operator[] (const std::string& name);
    IMF_EXPORT
    const Channel& operator[] (const std::string& name) const;

    IMF_EXPORT
    Channel* findChannel (const char name[]);
    IMF_EXPORT
    const Channel* findChannel (const char name[]) const;

    IMF_EXPORT
    Channel* findChannel (const std::string& name);
    IMF_EXPORT
    const Channel* findChannel (const std::string& name) const;

    //-------------------------------------------
    // Iterator-style access to existing channels
    //-------------------------------------------

    typedef std::map<Name, Channel> ChannelMap;

    class Iterator;
    class ConstIterator;

    IMF_EXPORT
    Iterator begin ();
    IMF_EXPORT
    ConstIterator begin () const;

    IMF_EXPORT
    Iterator end ();
    IMF_EXPORT
    ConstIterator end () const;

    IMF_EXPORT
    Iterator find (const char name[]);
    IMF_EXPORT
    ConstIterator find (const char name[]) const;

    IMF_EXPORT
    Iterator find (const std::string& name);
    IMF_EXPORT
    ConstIterator find (const std::string& name) const;

    //-----------------------------------------------------------------
    // Support for image layers:
    //
    // In an image file with many channels it is sometimes useful to
    // group the channels into "layers", that is, into sets of channels
    // that logically belong together.  Grouping channels into layers
    // is done using a naming convention:  channel C in layer L is
    // called "L.C".
    //
    // For example, a computer graphic image may contain separate
    // R, G and B channels for light that originated at each of
    // several different virtual light sources.  The channels in
    // this image might be called "light1.R", "light1.G", "light1.B",
    // "light2.R", "light2.G", "light2.B", etc.
    //
    // Note that this naming convention allows layers to be nested;
    // for example, "light1.specular.R" identifies the "R" channel
    // in the "specular" sub-layer of layer "light1".
    //
    // Channel names that don't contain a "." or that contain a
    // "." only at the beginning or at the end are not considered
    // to be part of any layer.
    //
    // layers(lns)		sorts the channels in this ChannelList
    //				into layers and stores the names of
    //				all layers, sorted alphabetically,
    //				into string set lns.
    //
    // channelsInLayer(ln,f,l)	stores a pair of iterators in f and l
    // 				such that the loop
    //
    // 				for (ConstIterator i = f; i != l; ++i)
    // 				   ...
    //
    //				iterates over all channels in layer ln.
    //				channelsInLayer (ln, l, p) calls
    //				channelsWithPrefix (ln + ".", l, p).
    //
    //-----------------------------------------------------------------

    IMF_EXPORT
    void layers (std::set<std::string>& layerNames) const;

    IMF_EXPORT
    void channelsInLayer (
        const std::string& layerName, Iterator& first, Iterator& last);

    IMF_EXPORT
    void channelsInLayer (
        const std::string& layerName,
        ConstIterator&     first,
        ConstIterator&     last) const;

    //-------------------------------------------------------------------
    // Find all channels whose name begins with a given prefix:
    //
    // channelsWithPrefix(p,f,l) stores a pair of iterators in f and l
    // such that the following loop iterates over all channels whose name
    // begins with string p:
    //
    //		for (ConstIterator i = f; i != l; ++i)
    //		    ...
    //
    //-------------------------------------------------------------------

    IMF_EXPORT
    void
    channelsWithPrefix (const char prefix[], Iterator& first, Iterator& last);

    IMF_EXPORT
    void channelsWithPrefix (
        const char prefix[], ConstIterator& first, ConstIterator& last) const;

    IMF_EXPORT
    void channelsWithPrefix (
        const std::string& prefix, Iterator& first, Iterator& last);

    IMF_EXPORT
    void channelsWithPrefix (
        const std::string& prefix,
        ConstIterator&     first,
        ConstIterator&     last) const;

    //------------
    // Operator ==
    //------------

    IMF_EXPORT
    bool operator== (const ChannelList& other) const;

private:
    ChannelMap _map;
};

//----------
// Iterators
//----------

class IMF_EXPORT_TYPE ChannelList::Iterator
{
public:
    IMF_EXPORT
    Iterator ();
    IMF_EXPORT
    Iterator (const ChannelList::ChannelMap::iterator& i);

    IMF_EXPORT
    Iterator& operator++ ();
    IMF_EXPORT
    Iterator operator++ (int);

    IMF_EXPORT
    const char* name () const;
    IMF_EXPORT
    Channel& channel () const;

private:
    friend class ChannelList::ConstIterator;

    ChannelList::ChannelMap::iterator _i;
};

class IMF_EXPORT_TYPE ChannelList::ConstIterator
{
public:
    IMF_EXPORT
    ConstIterator ();
    IMF_EXPORT
    ConstIterator (const ChannelList::ChannelMap::const_iterator& i);
    IMF_EXPORT
    ConstIterator (const ChannelList::Iterator& other);

    IMF_EXPORT
    ConstIterator& operator++ ();
    IMF_EXPORT
    ConstIterator operator++ (int);

    IMF_EXPORT
    const char* name () const;
    IMF_EXPORT
    const Channel& channel () const;

private:
    friend bool operator== (const ConstIterator&, const ConstIterator&);
    friend bool operator!= (const ConstIterator&, const ConstIterator&);

    ChannelList::ChannelMap::const_iterator _i;
};

//-----------------
// Inline Functions
//-----------------

inline ChannelList::Iterator::Iterator () : _i ()
{
    // empty
}

inline ChannelList::Iterator::Iterator (
    const ChannelList::ChannelMap::iterator& i)
    : _i (i)
{
    // empty
}

inline ChannelList::Iterator&
ChannelList::Iterator::operator++ ()
{
    ++_i;
    return *this;
}

inline ChannelList::Iterator
ChannelList::Iterator::operator++ (int)
{
    Iterator tmp = *this;
    ++_i;
    return tmp;
}

inline const char*
ChannelList::Iterator::name () const
{
    return *_i->first;
}

inline Channel&
ChannelList::Iterator::channel () const
{
    return _i->second;
}

inline ChannelList::ConstIterator::ConstIterator () : _i ()
{
    // empty
}

inline ChannelList::ConstIterator::ConstIterator (
    const ChannelList::ChannelMap::const_iterator& i)
    : _i (i)
{
    // empty
}

inline ChannelList::ConstIterator::ConstIterator (
    const ChannelList::Iterator& other)
    : _i (other._i)
{
    // empty
}

inline ChannelList::ConstIterator&
ChannelList::ConstIterator::operator++ ()
{
    ++_i;
    return *this;
}

inline ChannelList::ConstIterator
ChannelList::ConstIterator::operator++ (int)
{
    ConstIterator tmp = *this;
    ++_i;
    return tmp;
}

inline const char*
ChannelList::ConstIterator::name () const
{
    return *_i->first;
}

inline const Channel&
ChannelList::ConstIterator::channel () const
{
    return _i->second;
}

inline bool
operator== (
    const ChannelList::ConstIterator& x, const ChannelList::ConstIterator& y)
{
    return x._i == y._i;
}

inline bool
operator!= (
    const ChannelList::ConstIterator& x, const ChannelList::ConstIterator& y)
{
    return !(x == y);
}

OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_EXIT

#endif
