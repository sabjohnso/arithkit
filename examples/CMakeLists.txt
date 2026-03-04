set(examples
  combinatorics
  hilbert_matrix
  compute_pi
  mandelbrot
  newton_autodiff
  root_isolation
  affine_vs_interval
  rotation_3d
  geometric_algebra
  oriented_geometry
  batch_polynomial
  sparsity_detection
  composition_algebra
  modular_arithmetic
  higher_order_ad
  polynomial_ops
  fixed_point_filter)

foreach(example ${examples})
  add_executable(${example} ${example}.cpp)
  target_link_libraries(${example} PRIVATE arithkit::header)
  set_target_properties(${example}
    PROPERTIES
    CXX_STANDARD ${arithkit_CXX_STANDARD})
endforeach()
