#!/bin/bash

# Define here the installation path for your platform
case "$HOSTNAME" in
	Arcteryx)
		install_path="$PWD/install"
		options="-DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF"
		optimus=true;;
	*) echo "Your platform is not defined in $0. Using default parameters."
		install_path="$PWD/install"
    options="-DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF"
    optimus=false
		;;
esac

# Functions
function build
{
  mkdir -p build &&
  cd build &&
  cmake -DCMAKE_INSTALL_PREFIX:PATH=$install_path	\
        $options                                  \
        .. &&
  make install &&
  cd ..
}
function launch
{
	if test $# -gt 0; then
		cfg=$1
	else
		cfg=data/params/config.xml 
	fi
	if $optimus; then 
		echo "Starting gpu ..."
		optirun ./install/bin/Reality $cfg
	#	primusrun ./install/bin/Reality $cfg
	else
		./install/bin/Reality $cfg
	fi
}
function remove
{
  rm -rfv $1
}
function help
{
  echo "Project generator"
  echo "Options are:"
  echo " -h, --help         show brief help"
  echo " -b, --build        build the project"
  echo " -c, --clean        clean the project"
  echo " --clear            clear the project (remove binaries)"
}

############################################
#                   MAIN                   #
############################################

if test $# -gt 0; then
  case "$1" in
    -h|--help)
      help
      exit 0
      ;;
    
    -b|--build)
      build
      exit 0
      ;;
      
    -c|--clean)
      remove "build"
      exit 0
      ;;
    
    --clear)
      remove "build install"
      exit 0
      ;;
      
    *)
      echo "invalid option -- '$1'"
      help
      exit 0
      ;;
  esac
fi

echo "Options $options"

build && launch
#help
