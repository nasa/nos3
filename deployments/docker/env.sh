cat << EOF
# DO NOT PUT ANY SENSITIVE CONFIGURATIONS HERE
DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO}
HEALTHCHECK_PORT=${HEALTHCHECK_PORT}

# Proxy configurations, the proxy values come for the environment, which is important for the vmmoc
HTTPS_PROXY=${HTTPS_PROXY}
HTTP_PROXY=${HTTP_PROXY}
NO_PROXY=${NO_PROXY}

# Maven specific configurations
MAVEN_REPO_LOCAL="/home/nos3/.nos3/.m2/repository"
MAVEN_HTTPS_PROXY="--settings ./settings.xml"

# 42 specific configurations
FORTYTWO_DISPLAY=":1"
FORTYTWO_GIT_URL="https://github.com/nasa-itc/42.git"
FORTYTWO_GIT_COMMIT="nos3-main"
FORTYTWO_GIT_FOLDER="42"
FORTYTWO_STARTUP_FOLDER="NOS3InOut"
FORTYTWO_RECOMPILE=false

# gsw specific configurations
GSW_SOFTWARE=yamcs
COMPONENT_DIR="/home/nos3/builds/nos3/components"

YAMCS_DATA_DIR="/home/nos3/.nos3/yamcs/target/yamcs/yamcs-data"
# YAMCS_ETC_DIR="/home/nos3/.nos3/yamcs/target/yamcs/yamcs-etc"
# YAMCS_CACHE_DIR="/home/nos3/.nos3/yamcs/target/yamcs/yam
YAMCS_GIT_URL=https://github.com/nasa-itc/yamcs-nos3.git
YAMCS_GIT_COMMIT=dev

# nos3 specific configurations
BASE_DIR="/opt/nasa-itc"

EOF