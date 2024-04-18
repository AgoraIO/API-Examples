

http_proxy=http://10.10.114.51:1080 https_proxy=http://10.10.114.51:1080 pod install --repo-update
if [ $? -eq 0 ]; then
    echo "pod success"
else
    echo "pod failed"
    exit 1
fi

script_path="$( cd "$(dirname "$0")" ; pwd -P )"
echo $script_path
current_datetime=$(date +'%Y-%m-%d_%H-%M-%S')  
echo $current_datetime
zip -r ${1}_OC_${current_datetime}.zip .