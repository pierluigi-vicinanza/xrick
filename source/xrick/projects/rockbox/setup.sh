#!/bin/bash
#
# This script will prepare your Rockbox source folder for xrick compilation.
#

SCRIPT_PATH="${0%/*}"
if [[ ("$0" != "$SCRIPT_PATH") && ("$SCRIPT_PATH" != "") ]]; then 
    cd $SCRIPT_PATH
fi
 
function print_help
{
    echo "Usage: `basename $0` [options] directory"
    echo "Options:"
    echo "  -h, --help               Display this information"
    echo "  -p, --path <directory>   Use <directory> as your Rockbox source path"
}

ROCKBOX_DIR=""

# parse arguments
while [[ $# > 0 ]]
do
    case "$1" in
        -p|--path)
            shift
            ROCKBOX_DIR="$1"
        ;;
        -h|--help)
            print_help
            exit 0
        ;;  
        *)
        ;;
    esac
    shift
done

if [[ "$ROCKBOX_DIR" == "" ]]; then
    echo "Error: Rockbox source folder not specified"
    print_help
    exit 1
fi

# stop script on error
set -e

echo "Copying xrick sources to $ROCKBOX_DIR/apps/plugins/xrick ..."
cp -r ../../../xrick $ROCKBOX_DIR/apps/plugins
cp ./config.h ./SOURCES ./xrick.make $ROCKBOX_DIR/apps/plugins/xrick

if [[ `tail $ROCKBOX_DIR/apps/plugins/SUBDIRS -n 1` != "xrick" ]]; then
    echo "Appending xrick to $ROCKBOX_DIR/apps/plugins/SUBDIRS ..."
    echo "xrick" >> $ROCKBOX_DIR/apps/plugins/SUBDIRS
fi

if [[ `tail $ROCKBOX_DIR/apps/plugins/CATEGORIES -n 1` != "xrick,games" ]]; then
    echo "Appending xrick to $ROCKBOX_DIR/apps/plugins/CATEGORIES ..."
    echo "xrick,games" >> $ROCKBOX_DIR/apps/plugins/CATEGORIES
fi

echo "Done."
