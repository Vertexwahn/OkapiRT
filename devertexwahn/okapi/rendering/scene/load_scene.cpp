/*
 *  SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/scene/load_scene.h"

//#include "okapi/rendering/bsdf/dielectric.hpp"
//#include "okapi/rendering/bsdf/diffuse.hpp"
//#include "okapi/rendering/bsdf/mirror.hpp"
//#include "okapi/rendering/bsdf/phong.hpp"
//#include "okapi/rendering/integrator/ambient_occlusion.h"
//#include "okapi/rendering/integrator/aov.hpp"
//#include "okapi/rendering/integrator/hit_integrator.hpp"
//#include "okapi/rendering/integrator/next_integrator.hpp"
//#include "okapi/rendering/integrator/normal_integrator.hpp"
//#include "okapi/rendering/integrator/path_tracing_diffuse.hpp"
//#include "okapi/rendering/integrator/path_tracing_naive_diffuse.hpp"
//#include "okapi/rendering/integrator/simple_integrator.hpp"
//#include "okapi/rendering/integrator/whitted_integrator.hpp"
#include "okapi/rendering/intersector/embree_intersector.hpp"
//#include "okapi/rendering/intersector/octree_intersector.hpp"
//#include "okapi/rendering/emitter/area_light.hpp"
//#include "okapi/rendering/shape/serialized_mesh.h"
#include "okapi/rendering/shape/sphere.h"
#include "okapi/rendering/shape/triangle_mesh.h"
#include "okapi/rendering/sensor/sensor.hpp"
//#include "okapi/rendering/texture/alpha_blended_texture.hpp"
//#include "okapi/rendering/texture/bitmap.hpp"
//#include "okapi/rendering/texture/checkerboard.hpp"
//#include "okapi/rendering/texture/texture.hpp"

//#include "flatland/rendering/intersector/brute_force_intersector.hpp"
#include "flatland/rendering/property_set.h"
#include "flatland/rendering/sampler.hpp"

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
    ObjectFactory<PropertySet> sf;

    // intersector/accelerator
    //sf.register_class<BruteForceIntersector3f>("brute_force");
    sf.register_class<EmbreeIntersector>("embree");
    //sf.register_class<OctreeIntersector>("octree");

    // sampler
    //sf.register_class<IndependentSampler>("independent");
    //sf.register_class<ConstantSampler>("static");

    // integrator
    /*
    sf.register_class<AOVIntegrator3f>("aov");
    sf.register_class<AmbientOcclusion3f>("ambient_occlusion");
    sf.register_class<HitIntegrator3f>("hit");
    sf.register_class<NextIntegrator3f>("next");
    sf.register_class<NormalIntegrator3f>("normal");
    sf.register_class<PathTracingDiffuse3f>("pt_diffuse");
    sf.register_class<PathTracingNaiveDiffuse3f>("naive_diffuse");
    sf.register_class<SimpleIntegrator3f>("simple");
    sf.register_class<WhittedIntegrator3f>("whitted");
    */

    // shapes
    sf.register_class<Sphere3f>("sphere");
    sf.register_class<TriangleMesh3f>("obj");
    //sf.register_class<SerializedMesh3f>("serialized");

    // BSDFs
    /*
    sf.register_class<Dielectric>("dielectric");
    sf.register_class<Diffuse>("diffuse");
    sf.register_class<Mirror>("mirror");
    sf.register_class<Phong>("phong");
    */

    // emitters
    //sf.register_class<AreaLight3f>("area");

    // textures
    //sf.register_class<AlphaBlendedTexture>("mix_texture");
    //sf.register_class<BitmapTexture>("bitmap");
    //sf.register_class<Checkerboard>("checkerboard");

    // filter
    sf.register_class<BoxFilter>("box");
    sf.register_class<GaussianFilter>("gaussian");
    sf.register_class<TentFilter>("tent");

    return load_scene<float, 3>(filename, sf, override_scene_properties);
}

DE_VERTEXWAHN_END_NAMESPACE
