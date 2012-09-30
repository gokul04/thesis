set -xv
INPUT_LOGICAL_FILE_NAME="input"
OUTPUT_LOGICAL_FILE_NAME="output"
ZIPFILE_LOGICAL_FILE_NAME="convert.zip"
INPUT_FILE_NAME=`boinc resolve_filename "${INPUT_LOGICAL_FILE_NAME}"`
OUTPUT_FILE_NAME=`boinc resolve_filename "${OUTPUT_LOGICAL_FILE_NAME}"`
ZIPFILE_FILE_NAME=`boinc resolve_filename "${ZIPFILE_LOGICAL_FILE_NAME}"`
unzip ${ZIPFILE_FILE_NAME}
chmod +x ./matrix
./matrix -i ${INPUT_FILE_NAME} -o  ${OUTPUT_FILE_NAME} 1>&2
boinc finish 0
