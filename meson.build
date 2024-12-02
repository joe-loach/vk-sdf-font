project(
    'vk-sdf-font',
    'cpp',
    default_options: [
        'cpp_std=c++23',
        'warning_level=3'
    ]
)

ttf_parser = subproject('ttf-parser')
tff_parser_dep = ttf_parser.get_variable('ttf_parser_dep')

executable(
    'vk-sdf-font',
    'main.cpp',
    dependencies: [
        tff_parser_dep
    ]
)
