project(
    'vk-sdf-font',
    'cpp',
    default_options: [
        'cpp_std=c++23',
        'warning_level=3'
    ]
)

cmake = import('cmake')

vk_bootstrap = cmake.subproject('vk-bootstrap')

vk_bootstrap_dep = vk_bootstrap.dependency('vk-bootstrap')

glm_dep = dependency('glm')
glfw_dep = dependency('glfw3')

ttf_parser = subproject('ttf-parser')
tff_parser_dep = ttf_parser.get_variable('ttf_parser_dep')

executable(
    'vk-sdf-font',
    'main.cpp',
    dependencies: [
        tff_parser_dep,
        glm_dep,
        vk_bootstrap_dep,
        glfw_dep
    ]
)
