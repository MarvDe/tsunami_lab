##
# @author Alexander Breuer (alex.breuer AT uni-jena.de)
#
# @section DESCRIPTION
# Entry-point for builds.
##
import SCons
import os


print( '####################################' )
print( '### Tsunami Lab                  ###' )
print( '###                              ###' )
print( '### https://scalable.uni-jena.de ###' )
print( '####################################' )
print()
print('runnning build script')

# configuration
vars = Variables()

vars.AddVariables(
  EnumVariable( 'mode',
                'compile modes, option \'san\' enables address and undefined behavior sanitizers',
                'release',
                allowed_values=('release', 'debug', 'release+san', 'debug+san', 'optimization' )
              )
)

# exit in the case of unknown variables
if vars.UnknownVariables():
  print( "build configuration corrupted, don't know what to do with: " + str(vars.UnknownVariables().keys()) )
  exit(1)

# create environment
env = Environment( variables = vars, ENV=os.environ )

# generate help message
Help( vars.GenerateHelpText( env ) )

# select compiler
if ("CXX" in os.environ):
  env.Replace( CXX = os.environ["CXX"] )
print("using compiler: ", env["CXX"])

# add default flags
env.Append( CXXFLAGS = [ '-std=c++17',
                         '-Wall',
                         '-Wextra',
                         '-Wpedantic',
                         '-Wno-keyword-macro',
                         ] )

# set optimization mode
if 'debug' in env['mode']:
  env.Append( CXXFLAGS = [ '-g',
                           '-O0' ] )
else:
  if 'optimization' in env['mode']:
    env.Append( CXXFLAGS = ['-O3',
                            '-ffast-math',
                            '-march=native',
                            #'-flto'
                            ] )

  else:
    env.Append( CXXFLAGS = [ '-O2' ] )


# add sanitizers
if 'san' in  env['mode']:
  env.Append( CXXFLAGS =  [ '-g',
                            '-fsanitize=float-divide-by-zero',
                            '-fsanitize=bounds',
                            '-fsanitize=address',
                            '-fsanitize=undefined',
                            '-fno-omit-frame-pointer' ] )
  env.Append( LINKFLAGS = [ '-g',
                            '-fsanitize=address',
                            '-fsanitize=undefined' ] )
else:
  env.Append( CXXFLAGS = [ '-Werror' ] )

# add Catch2
env.Append( CXXFLAGS = [ '-isystem', 'submodules/Catch2/single_include' ] )

# add yaml-cpp
env.Append( CXXFLAGS = [ '-isystem', 'submodules/yaml-cpp/include' ] )
env.Append( CPPPATH=['submodules/yaml-cpp/include'])
env.Append( LIBPATH=['submodules/yaml-cpp/build'])
env.Append( LIBS=['yaml-cpp'])

# add openMP
env.Append( CXXFLAGS = ['-fopenmp'])
env.Append( LINKFLAGS = ['-fopenmp'])

# add netcdf via pkg-config
env.ParseConfig('nc-config --cflags --libs')

conf = Configure(env)
if not conf.CheckCHeader('netcdf.h'):
    print("Did not find netcdf.h, exiting!")
    Exit(1)

env = conf.Finish()

# get source files
VariantDir( variant_dir = 'build/src',
            src_dir     = 'src' )

env.sources = []
env.tests = []

Export('env')
SConscript( 'build/src/SConscript' )
Import('env')

env.Program( target = 'build/tsunami_lab',
             source = env.sources + env.standalone )

env.Program( target = 'build/tests',
             source = env.sources + env.tests )