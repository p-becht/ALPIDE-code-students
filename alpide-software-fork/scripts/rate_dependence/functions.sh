#!/bin/bash
###################################################################################################
# library containing functionality used by different measurement scripts
###################################################################################################

###################################################################################################
### determine 'run conditions'
#
# to be used with eval
#
determine_run_conditions ()
{
    # date
    DATE=`date +%Y-%m-%d_%H-%M-%S`

    # git
    GIT_HASH=`git log -1 --pretty=format:%H`
    GIT_ABBREV_HASH=`git log -1 --pretty=format:%h`
    GIT_CHANGES=`git diff --shortstat 2> /dev/null | tail -n1`
    if [ -z "${GIT_CHANGES}" ]
    then
        GIT_MODIFIED="0"
    else
        GIT_MODIFIED="1"
    fi
    if [ "${GIT_MODIFIED}" -eq "0" ]
    then
        GIT_INFO=${GIT_ABBREV_HASH}
    else
        GIT_INFO=${GIT_ABBREV_HASH}"M"
    fi
    env | grep GIT

    # needed to pass the variables to the script calling the function
    echo "export DATE=$DATE"
    echo "export GIT_HASH=$GIT_HASH"
    echo "export GIT_ABBREV_HASH=$GIT_ABBREV_HASH"
    echo "export GIT_MODIFIED=$GIT_MODIFIED"
    echo "export GIT_INFO=$GIT_INFO"
}

###################################################################################################
### create output folder
create_output_folder()
{
    local OUTPUT_PATH=$1
    mkdir -p $OUTPUT_PATH
    if [ ! -d $OUTPUT_PATH ]
    then
        echo "ERROR: could not create output folder"
    fi
}

###################################################################################################
### store git diff
store_git_diff()
{
    local OUTPUT_PATH=$1
    if [ ${GIT_MODIFIED} == "1" ]
    then
        git diff > ${OUTPUT_PATH}/git.diff
    fi
}

###################################################################################################
### store environment variables / configuration
store_env_config()
{
    local OUTPUT_PATH=$1
    tail -n +1 config* > ${OUTPUT_PATH}/configs.txt 2> /dev/null
    env > ${OUTPUT_PATH}/env.txt
}

###################################################################################################
### check ROOT availability
check_root() {
    command -v root-config >/dev/null 2>&1 || {
        echo >&2 "ROOT is neeed for running! Aborting.";
        exit 1;
    }
}
###################################################################################################
### precompile root library
precompile_lib() {
    # check for number of arguments in function call
    n_args=$#
    # if one argument, compile without loading data-structures
    if [ $n_args -eq 1 ]
    then
        local filename=$1
        local extension="${filename##*.}"
        local filename="${filename%.*}"
        rm -vfr ${filename}_${extension}*
        root -l -b <<EOF
exit(gSystem->CompileMacro("${filename}.${extension}", "fgk"))
EOF
        if [ $? -ne 1 ]
        then
            echo "Compilation of ${filename}.${extension} failed! exiting..."
            exit 1
        fi
    fi
    # if two arguments, compile without loading data-structures
    if [ $n_args -eq 2 ]
    then
        local filename=$1
        local libpath=$(readlink -f $2)
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$libpath
        local extension="${filename##*.}"
        local filename="${filename%.*}"
        rm -vfr ${filename}_${extension}*
        root -l -b <<EOF
.x $libpath/load_classes.C+g
exit(gSystem->CompileMacro("${filename}.${extension}", "fgk"))
EOF
        if [ $? -ne 1 ]
        then
            echo "Compilation of ${filename}.${extension} failed! exiting..."
            exit 1
        fi
    fi
}
