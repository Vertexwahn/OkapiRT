/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Okapi_RenderState_4a221adf_1203_499f_8081_3634a692b45a_hpp
#define De_Vertexwahn_Okapi_RenderState_4a221adf_1203_499f_8081_3634a692b45a_hpp

#include "core/namespace.hpp"

//DE_VERTEXWAHN_BEGIN_NAMESPACE

enum class RenderState {
    /**
     * Start state - before anything happens
     */
    Start,

    /**
     * Loading the scene is in progress
     */
    Loading,

    /**
     * Rendering in progress
     */
    Rendering,

    /**
     * Store results on disk as OpenEXR file
     */
    StoreResults,

    /**
     * Everything is finished
     */
    Done
};

//DE_VERTEXWAHN_END_NAMESPACE

#endif // end define De_Vertexwahn_Okapi_RenderState_4a221adf_1203_499f_8081_3634a692b45a_hpp
