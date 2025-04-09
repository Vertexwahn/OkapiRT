/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef Okapi_loadScene_48a28c98_4cfb_4ceb_85e3_118d91191028_h
#define Okapi_loadScene_48a28c98_4cfb_4ceb_85e3_118d91191028_h

#include "core/object_factory.hpp"

#include "core/exception.hpp"
#include "core/namespace.hpp"
#include "core/reference_counted.hpp"
#include "flatland/rendering/scene/load_scene.hpp"
#include "flatland/rendering/scene/scene.hpp"
#include "okapi/rendering/sensor/film.hpp"
#include "okapi/rendering/sensor/reconstruction_filter.hpp"

#include "pugixml.hpp"

#include <charconv>
#include <clocale>
#include <filesystem>
#include <fstream>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

DE_VERTEXWAHN_BEGIN_NAMESPACE

void create_child_objects(
        const std::string& parent_path,
        pugi::xml_node &parent,
        const ObjectFactory<PropertySet> &object_factory,
        PropertySet &out_ps);

ReferenceCounted<Scene3f>
load_scene3f(std::string_view filename, const PropertySet &override_scene_properties = {});

// construct filter from XML film element. If it does not exist construct a default filter.
template <typename ScalarType>
ReferenceCounted<ReconstructionFilterType<ScalarType>> read_filter(const pugi::xml_node& xmlFilm, const ObjectFactory<PropertySet>& object_factory) {
    ReferenceCounted<ReconstructionFilterType<ScalarType>> filter = nullptr;

    auto xml_filter = xmlFilm.child("rfilter");
    if (xml_filter) {
        std::string filter_type = xml_filter.attribute("type").as_string();
        PropertySet filter_ps = read_all_properties(xml_filter);
        filter = std::dynamic_pointer_cast<ReconstructionFilter>(object_factory.create_instance(filter_type, filter_ps));
    }
    else {
        filter = make_reference_counted<GaussianFilterType<ScalarType>>(PropertySet{});
    }

    return filter;
}

template <typename ScalarType, unsigned int Dimension>
ReferenceCounted<SceneType<ScalarType, Dimension>> load_scene(
        std::string_view filename,
        const ObjectFactory<PropertySet>& object_factory,
        const PropertySet& override_scene_properties) {
    // https://stackoverflow.com/questions/12316972/how-to-tweak-stdstod-string-to-double-for-decimal-separator-and-number-of-di
    // save locale setting
    const auto old_locale = std::setlocale(LC_NUMERIC, nullptr);

    // force '.' as the radix point
    std::setlocale(LC_NUMERIC,"C");

    pugi::xml_document doc;
    auto result = doc.load_file(filename.data());

    if (!result)
        return nullptr;

    auto scene = make_reference_counted<SceneType<ScalarType, Dimension>>();

    std::filesystem::path p(filename);
    bool integrator_tag_found = false;

    for (pugi::xml_node scene_node : doc.root()) {
        if (std::string(scene_node.name()) == "scene") {
            for (pugi::xml_node scene_elements : scene_node) {
                if (std::string(scene_elements.name()) == "sensor") {
                    // construct film
                    auto xml_film = scene_elements.child("film");

                    PropertySet film_ps = read_all_properties(xml_film);
                    auto filter = read_filter<ScalarType>(xml_film, object_factory);
                    film_ps.add_property("filter", filter);

                    if(override_scene_properties.has_property("film_filename")) {
                        film_ps.set_property("filename", override_scene_properties.get_property<std::string>("film_filename"));
                    }

                    auto film = make_reference_counted<Film>(film_ps);

                    // construct sensor
                    PropertySet sensor_ps = read_all_properties(scene_elements);
                    sensor_ps.add_property("film", film);
                    auto sensor = make_reference_counted<SensorType<ScalarType, Dimension>>(sensor_ps);

                    auto xml_transform = scene_elements.child("transform");
                    if (xml_transform) {
                        auto transform = read_transform<Dimension>(xml_transform);
                        sensor->set_transform(transform);
                    }

                    scene->set_sensor(sensor);

                    auto xml_sampler = scene_elements.child("sampler");
                    if(xml_sampler) {
                        std::string type = xml_sampler.attribute("type").as_string();

                        auto ps = read_all_properties(xml_sampler);

                        if(override_scene_properties.has_property("samples_per_pixel")) {
                            int spp = override_scene_properties.get_property<int>("samples_per_pixel");
                            if(ps.has_property("sample_count")) {
                                ps.set_property("sample_count", spp);
                            }
                            else {
                                ps.set_property("sample_count", spp);
                            }
                        }

                        auto sampler = std::dynamic_pointer_cast<SamplerType<ScalarType>>(
                                object_factory.create_instance(type, ps));
                        scene->set_sampler(sampler);
                    }
                }

                if (std::string(scene_elements.name()) == "integrator") {
                    std::string type = scene_elements.attribute("type").as_string();
                    if(override_scene_properties.has_property("integrator")) {
                        type = override_scene_properties.get_property<std::string>("integrator");
                    }

                    auto ps = read_all_properties(scene_elements);

                    try {
                        auto integrator = std::dynamic_pointer_cast<IntegratorType<ScalarType, Dimension>>(
                                object_factory.create_instance(type, ps));
                        scene->set_integrator(integrator);
                    } catch(const PropertyDoesNotExistException& ex) {  // Todo: figure out if std::exception can be used here
                        throw ex;
                    }
                    catch (...) {
                        throw Exception("Integrator missing");
                    }

                    integrator_tag_found = true;
                }

                if (std::string(scene_elements.name()) == "shape") {
                    std::string type = scene_elements.attribute("type").as_string();

                    auto ps = read_all_properties(scene_elements);

                    if (type == "polygon" || type == "obj" || type == "triangle_mesh" || type == "serialized" ) {
                        ps.add_property("parent_path", p.parent_path().string());
                    }

                    auto transform = identity<ScalarType>();

                    auto xml_transform = scene_elements.child("transform");
                    if (xml_transform) {
                        transform = read_transform<Dimension>(xml_transform);
                    }

                    ps.add_property("transform", transform);

                    auto shape = std::dynamic_pointer_cast<ShapeType<ScalarType, Dimension>>(
                            object_factory.create_instance(type, ps));

                    auto xml_bsdf = scene_elements.child("bsdf");
                    if(xml_bsdf) {
                        std::string str_bsdf_type = xml_bsdf.attribute("type").as_string();
                        if(str_bsdf_type == "") {
                            throw Exception("No empty BSDF type allowed.");
                        }
                        if (xml_bsdf) {
                            auto bsdf_ps = read_all_properties(xml_bsdf);
                            // maybe some BSDFs want to read the filesystem - therefore we provide a parent path
                            std::string parent_path = p.parent_path().string();

                            create_child_objects(parent_path, xml_bsdf, object_factory, bsdf_ps);

                            auto bsdf = std::dynamic_pointer_cast<BSDFType<ScalarType, Dimension>>(
                                    object_factory.create_instance(str_bsdf_type, bsdf_ps));
                            shape->set_bsdf(bsdf);
                        }
                    }

                    auto xml_emitter = scene_elements.child("emitter");
                    if(xml_emitter) {
                        std::string str_emitter_type = xml_emitter.attribute("type").as_string();
                        auto emitter_ps = read_all_properties(xml_emitter);
                        auto emitter = std::dynamic_pointer_cast<EmitterType<ScalarType, Dimension>>(
                            object_factory.create_instance(str_emitter_type, emitter_ps)
                        );
                        shape->set_emitter(emitter);
                    }

                    scene->add_shape(shape);
                }

                if (std::string(scene_elements.name()) == "intersector") {
                    std::string type = scene_elements.attribute("type").as_string();

                    if(override_scene_properties.has_property("intersector")) {
                        type = override_scene_properties.get_property<std::string>("intersector");
                    }

                    auto intersector = std::dynamic_pointer_cast<IntersectorType<ScalarType, Dimension>>(
                            object_factory.create_instance(type, PropertySet{}));
                    scene->set_intersector(intersector);
                }
            }
        } else {
            throw Exception("scene_node xml tag missing");
        }
    }

    if (!integrator_tag_found) {
        throw Exception("Integrator missing");
    }

    if (!scene->has_intersector()) {
        std::string type = "brute_force";

        if(override_scene_properties.has_property("intersector")) {
            type = override_scene_properties.get_property<std::string>("intersector");
        }

        auto intersector = std::dynamic_pointer_cast<IntersectorType<ScalarType, Dimension>>(
                object_factory.create_instance(type, PropertySet{}));
        scene->set_intersector(intersector);
    }

    auto intersector = scene->intersector();
    intersector->build_acceleration_structure(scene->shapes());

    if (scene->sampler() == nullptr) {
        scene->set_sampler(make_reference_counted<IndependentSamplerType<ScalarType>>(PropertySet{}));
    }

    // restore locale setting
    std::setlocale(LC_NUMERIC, old_locale);

    return scene;
}

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define Okapi_loadScene_48a28c98_4cfb_4ceb_85e3_118d91191028_h
