

http_proxy=http://10.10.114.51:1080 https_proxy=http://10.10.114.51:1080 pod install --repo-update
if [ $? -eq 0 ]; then
    echo "pod success"
else
    echo "pod failed"
    exit 1
fi

script_path="$( cd "$(dirname "$0")" ; pwd -P )"
echo $script_path
today=$(date +"%Y-%m-%d")
project_name=$(echo $script_path | rev | cut -d'/' -f1 | rev)
echo $project_name
zip -r ${1}_${project_name}_${today}.zip .