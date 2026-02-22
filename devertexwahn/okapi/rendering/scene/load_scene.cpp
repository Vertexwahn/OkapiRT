/*
 *  SPDX-FileCopyrightText: Copyright 2022-2026 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/scene/load_scene.hpp"

#include "okapi/rendering/sensor/sensor.hpp"
#include "okapi/rendering/scene/registry.hpp"

#include "flatland/rendering/property_set.h"

#include "core/logging.hpp"
#include "core/object_factory.hpp"

#include <set>

DE_VERTEXWAHN_BEGIN_NAMESPACE

// creates recursive all objects
void create_child_objects(
        const std::string& parent_path,
        pugi::xml_node &parent,
        const ObjectFactory<PropertySet>& object_factory,
        PropertySet& out_ps) {

    for(pugi::xml_node &node: parent.children()) {
        // skip properties tags
        std::set<std::string> property_names {"rgb", "string"};

        if(property_names.contains(node.name())) {
            continue;
        }

        // create an object
        std::string str_type = node.attribute("type").as_string();

        if(str_type.empty()) {
            LOG_WARNING_WITH_LOCATION("No type provided {}", node.name());
            continue;
        }

        PropertySet ps;
        read_all_properties(node, ps);
        ps.add_property("parent_path", parent_path); // in the case some object wants to read some data form disk

        // create recursive object
        create_child_objects(parent_path, node, object_factory, ps);

        ReferenceCounted<Object> object = object_factory.create_instance(str_type, ps);

        out_ps.add_property(node.name(), object);
    }
}

ReferenceCounted<Scene3f> load_scene3f(std::string_view filename, const PropertySet& override_scene_properties) {
    ObjectFactory<PropertySet>::set_registration_callback(register_okapi_plugins);
    ObjectFactory<PropertySet>& object_factory = ObjectFactory<PropertySet>::instance();

    return load_scene<float, 3>(filename, object_factory, override_scene_properties);
}

DE_VERTEXWAHN_END_NAMESPACE
