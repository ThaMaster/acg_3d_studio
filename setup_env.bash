export VR_PATH=$( cd "$(dirname "$BASH_SOURCE[0]}" )" && pwd )
export DEPENDENCY_PATH=${VR_PATH}/installed
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${DEPENDENCY_PATH}/lib
export PATH=${PATH}:${DEPENDENCY_PATH}/bin
export FREETYPE_DIR=${DEPENDENCY_PATH}/installed