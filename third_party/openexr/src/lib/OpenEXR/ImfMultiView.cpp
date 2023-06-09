//
// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) Weta Digital, Ltd and Contributors to the OpenEXR Project.
//

//-----------------------------------------------------------------------------
//
//	Functions related to accessing channels
//	and views in multi-view OpenEXR files.
//
//-----------------------------------------------------------------------------

#include <ImfMultiView.h>

using namespace std;
#include "ImfNamespace.h"

OPENEXR_IMF_INTERNAL_NAMESPACE_SOURCE_ENTER

namespace
{

StringVector
parseString (string name, char c = '.')
{
    //
    // Turn name into a list of strings, separating
    // on char 'c' with whitespace stripped.
    //

    StringVector r;

    while (name.size () > 0)
    {
        size_t s   = name.find (c);
        string sec = name.substr (0, s);

        //
        // Strip spaces from beginning
        //

        while (sec.size () > 0 && sec[0] == ' ')
            sec.erase (0, 1);

        //
        // Strip spaces from end
        //

        while (sec.size () > 0 && sec[sec.size () - 1] == ' ')
            sec.erase (sec.size () - 1);

        r.push_back (sec);

        //
        // Strip off name including ending 'c'
        //

        if (s == name.npos)
            name = "";
        else
            name = name.substr (s + 1);
    }

    return r;
}

int
viewNum (const string& view, const StringVector& multiView)
{
    //
    // returns which view number is called 'view'
    // returns -1 if no member of multiView is 'view'
    // (i.e. if viewNum() returns -1, 'view' isn't a view name
    //       if viewNum() returns 0, 'view' is the default view
    // otherwise, it's some other (valid) view
    //

    for (size_t i = 0; i < multiView.size (); ++i)
    {
        if (multiView[i] == view) return i;
    }

    return -1;
}

} // namespace

string
defaultViewName (const StringVector& multiView)
{
    if (multiView.size () > 0)
        return multiView[0];
    else
        return "";
}

string
viewFromChannelName (const string& channel, const StringVector& multiView)
{
    //
    // Given the name of a channel, return the name of the view to
    // which it belongs.
    //

    //
    // View name is penultimate section of name separated by periods ('.'s)
    //

    StringVector s = parseString (channel, '.');

    if (s.size () == 0) return ""; // nothing in, nothing out

    if (s.size () == 1)
    {
        //
        // Return default view name.
        // The rules say ALL channels with no periods
        // in the name belong to the default view.
        //

        return multiView[0];
    }
    else
    {
        //
        // size >= 2 - the last part is the channel name,
        // the next-to-last part is the view name.
        // Check if that part of the name really is
        // a valid view and, if it is, return it.
        //

        const string& viewName = s[s.size () - 2];

        if (viewNum (viewName, multiView) >= 0)
            return viewName;
        else
            return ""; // not associated with any particular view
    }
}

ChannelList
channelsInView (
    const string&       viewName,
    const ChannelList&  channelList,
    const StringVector& multiView)
{
    //
    // Return a list of all channels belonging to view viewName.
    //

    ChannelList q;

    for (ChannelList::ConstIterator i = channelList.begin ();
         i != channelList.end ();
         ++i)
    {
        //
        // Get view name for this channel
        //

        string view = viewFromChannelName (i.name (), multiView);

        //
        // Insert channel into q if it's a member of view viewName
        //

        if (view == viewName) q.insert (i.name (), i.channel ());
    }

    return q;
}

ChannelList
channelsInNoView (const ChannelList& channelList, const StringVector& multiView)
{
    //
    // Return a list of channels not associated with any named view.
    //

    return channelsInView ("", channelList, multiView);
}

bool
areCounterparts (
    const string&       channel1,
    const string&       channel2,
    const StringVector& multiView)
{
    //
    // Given two channels, return true if they are the same
    // channel in two different views.
    //

    StringVector chan1 = parseString (channel1);
    size_t       size1 = chan1.size (); // number of SECTIONS in string
                                        // name (not string length)

    StringVector chan2 = parseString (channel2);
    size_t       size2 = chan2.size ();

    if (size1 == 0 || size2 == 0) return false;

    //
    // channel1 and channel2 can't be counterparts
    // if either channel is in no view.
    //

    if (size1 > 1 && viewNum (chan1[size1 - 2], multiView) == -1) return false;

    if (size2 > 1 && viewNum (chan2[size2 - 2], multiView) == -1) return false;

    if (viewFromChannelName (channel1, multiView) ==
        viewFromChannelName (channel2, multiView))
    {
        //
        // channel1 and channel2 are not counterparts
        // if they are in the same view.
        //

        return false;
    }

    if (size1 == 1)
    {
        //
        // channel1 is a default channel - the channels will only be
        // counterparts if channel2 is of the form <view>.<channel1>
        //

        return size2 == 2 && chan1[0] == chan2[1];
    }

    if (size2 == 1)
    {
        //
        // channel2 is a default channel - the channels will only be
        // counterparts if channel1 is of the form <view>.<channel2>
        //

        return size1 == 2 && chan2[0] == chan1[1];
    }

    //
    // Neither channel is a default channel.  To be counterparts both
    // channel names must have the same number of components, and
    // all components except the penultimate one must be the same.
    //

    if (size1 != size2) return false;

    for (size_t i = 0; i < size1; ++i)
    {
        if (i != size1 - 2 && chan1[i] != chan2[i]) return false;
    }

    return true;
}

ChannelList
channelInAllViews (
    const string&       channelName,
    const ChannelList&  channelList,
    const StringVector& multiView)
{
    //
    // Given the name of a channel, return a
    // list of the same channel in all views.
    //

    ChannelList q;

    for (ChannelList::ConstIterator i = channelList.begin ();
         i != channelList.end ();
         ++i)
    {
        if (i.name () == channelName ||
            areCounterparts (i.name (), channelName, multiView))
        {
            q.insert (i.name (), i.channel ());
        }
    }

    return q;
}

string
channelInOtherView (
    const string&       channelName,
    const ChannelList&  channelList,
    const StringVector& multiView,
    const string&       otherViewName)
{
    //
    // Given the name of a channel in one view, return the
    // corresponding channel name for view otherViewName.
    //

    for (ChannelList::ConstIterator i = channelList.begin ();
         i != channelList.end ();
         ++i)
    {
        if (viewFromChannelName (i.name (), multiView) == otherViewName &&
            areCounterparts (i.name (), channelName, multiView))
        {
            return i.name ();
        }
    }

    return "";
}

string
insertViewName (const string& channel, const StringVector& multiView, int i)
{
    //
    // Insert multiView[i] into the channel name if appropriate.
    //

    StringVector s = parseString (channel, '.');

    if (s.size () == 0) return ""; // nothing in, nothing out

    if (s.size () == 1 && i == 0)
    {
        //
        // Channel in the default view, with no periods in its name.
        // Do not insert view name.
        //

        return channel;
    }

    //
    // View name becomes penultimate section of new channel name.
    //

    string newName;

    for (size_t j = 0; j < s.size (); ++j)
    {
        if (j < s.size () - 1)
            newName += s[j] + ".";
        else
            newName += multiView[i] + "." + s[j];
    }

    return newName;
}

string
removeViewName (const string& channel, const string& view)
{
    StringVector s = parseString (channel, '.');

    if (s.size () == 0) return ""; // nothing in, nothing out

    if (s.size () == 1)
    {
        //
        // Channel in the default view, since no periods in its name.
        // No viewname to remove
        //

        return channel;
    }

    string newName;
    for (size_t j = 0; j < s.size (); ++j)
    {
        // only add the penultimate string part
        // if it doesn't match the view name
        if (j + 2 != s.size () || s[j] != view)
        {
            newName += s[j];
            if (j + 1 != s.size ()) { newName += "."; }
        }
    }

    return newName;
}

OPENEXR_IMF_INTERNAL_NAMESPACE_SOURCE_EXIT
