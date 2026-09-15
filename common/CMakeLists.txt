add_library(common STATIC)

target_sources(common 
    PRIVATE 
        src/figures/figure.cpp
        src/figures/sphere.cpp
        src/figures/cylinder.cpp
        src/materials/material.cpp
        src/materials/matte.cpp
        src/materials/metal.cpp
        src/materials/refractive.cpp
        src/sceneParser.cpp
        src/materials_render/material_render.cpp
        src/materials_render/matte_render.cpp
        src/materials_render/metal_render.cpp
        src/materials_render/refractive_render.cpp
        src/render_figures/hittable.cpp
        src/sceneRender.cpp
        src/config_parser.cpp
        src/rayTracer.cpp
        src/color.cpp
        src/camera.cpp
        src/bvh/bvh_node.cpp
)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(common PUBLIC Microsoft.GSL::GSL TBB::tbb)
