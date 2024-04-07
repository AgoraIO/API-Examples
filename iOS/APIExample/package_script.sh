
echo $1
echo $2

branch=${1//\//_}
echo branch: $branch

pod install --repo-update
if [ $? -eq 0 ]; then
    echo "pod success"
else
    echo "pod failed"
    exit 1
fi

script_path="$( cd "$(dirname "$0")" ; pwd -P )"
today=$(date +"%Y-%m-%d")
TARGET_NAME=${script_path##*/}
echo $TARGET_NAME
zip -r ${TARGET_NAME}_${branch}_${today}.zip .