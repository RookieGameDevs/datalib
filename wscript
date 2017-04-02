# -*- coding: utf-8 -*-
import sys


def options(opt):
    opt.load('compiler_c')

    opt.add_option(
        '--with-tests',
        action='store_true',
        help='build tests')

    opt.add_option(
        '--build-type',
        action='store',
        choices=['release', 'debug'],
        default='debug',
        help='build type (release or debug)')


def configure(cfg):
    cfg.load('compiler_c')

    cfg.env.append_unique('CFLAGS', '-std=c99')
    cfg.env.append_unique('CFLAGS', '-Wall')
    cfg.env.append_unique('CFLAGS', '-Werror')
    cfg.env.append_unique('CFLAGS', '-fPIC')

    cfg.env.with_tests = cfg.options.with_tests

    if cfg.options.build_type == 'debug':
        cfg.env.append_unique('CFLAGS', '-g')
        cfg.env.append_unique('DEFINES', 'DEBUG')
    else:
        cfg.env.append_unique('CFLAGS', '-O3')
        cfg.env.append_unique('DEFINES', 'NDEBUG')

    if cfg.options.with_tests:
        cfg.check_cfg(
            package='check',
            args='--libs --cflags',
            uselib_store='check')


def build(bld):
    # build library
    bld.shlib(
        target='data',
        source=bld.path.ant_glob('src/**/*.c'),
        install_path='${PREFIX}/lib')

    # build test suite
    if bld.env.with_tests:
        bld.program(
            target='test-suite',
            use=['data'],
            includes=['src'],
            uselib=['check'],
            rpath=bld.bldnode.abspath(),
            install_path=None,
            source=bld.path.ant_glob('tests/**/*.c'))

    bld.install_files('${PREFIX}/include', ['src/datalib.h'])
