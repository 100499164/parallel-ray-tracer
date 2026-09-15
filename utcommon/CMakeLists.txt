set(COMMON_SRC_FILES 
  "${CMAKE_SOURCE_DIR}/common/src/materials/matte.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/materials/metal.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/materials/refractive.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/figures/sphere.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/figures/cylinder.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/figures/figure.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/sceneParser.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/color.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/sceneRender.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/config_parser.cpp" 
  "${CMAKE_SOURCE_DIR}/common/src/rayTracer.cpp"

)

set(CURRENT_DIR_SRC_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/test_vector.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/materials/test_material.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/materials/test_matte.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/materials/test_metal.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/materials/test_refractive.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/figures/test_sphere.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/figures/test_cylinder.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/figures/test_figure.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_sceneParser.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_camera.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_color.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/render_figures/test_sphereRender.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/render_figures/test_cylinderRender.cpp"
  #"${CMAKE_CURRENT_SOURCE_DIR}/render_figures/test_hittable.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/render_materials/test_material_render.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/render_materials/test_matte_render.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/render_materials/test_metal_render.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/render_materials/tests_refractive_render.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_sceneRender.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_config_parser.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_image.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_ray.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_infocolision.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_scattercontext.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_raytracer.cpp"
)

add_unit_test_target(
  TARGET_NAME utcommon
  SOURCE_FILES ${COMMON_SRC_FILES} ${CURRENT_DIR_SRC_FILES}
  LIBRARY_FILTER common
  COVERAGE_DIR coverage-common
  LIBRARY_TO_LINK common
)
