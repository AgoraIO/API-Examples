WORKING_PATH=$1
APP_Project=$2
APP_TARGET=$3
BUNDLE_ID=$4
USERNAME=$5
PASSWORD=$6
ASCPROVIDER=$7
MODE=Release

echo "WORKING_PATH: ${WORKING_PATH}"
echo "APP_TARGET: ${APP_TARGET}"
echo "PROVIDER: ${ASCPROVIDER}"

cd ${WORKING_PATH}
echo `pwd`

rm -f *.ipa
rm -rf *.app
rm -f *.zip
rm -rf dSYMs
rm -rf *.dSYM
rm -f *dSYMs.zip
rm -rf *.xcarchive

Export_Plist_File=exportPlist.plist

BUILD_DATE=`date +%Y-%m-%d-%H.%M.%S`
ArchivePath=${APP_TARGET}-${BUILD_DATE}.xcarchive

TARGET_FILE=""
if [ ! -f "Podfile" ];then
TARGET_FILE="${APP_Project}.xcodeproj"
xcodebuild clean -project ${TARGET_FILE} -scheme "${APP_TARGET}" -configuration ${MODE}
xcodebuild -project ${TARGET_FILE} -scheme "${APP_TARGET}" -configuration ${MODE} -archivePath ${ArchivePath} archive
else
pod install
TARGET_FILE="${APP_Project}.xcworkspace"
xcodebuild clean -workspace ${TARGET_FILE} -scheme "${APP_TARGET}" -configuration ${MODE}
xcodebuild -workspace ${TARGET_FILE} -scheme "${APP_TARGET}" -configuration ${MODE} -archivePath ${ArchivePath} archive
fi

xcodebuild -exportArchive -exportOptionsPlist ${Export_Plist_File} -archivePath ${ArchivePath} -exportPath .

ls -alt

ditto -c -k --keepParent ${APP_TARGET}.app ${APP_TARGET}.zip
xcrun altool --notarize-app -f ${APP_TARGET}.zip --primary-bundle-id ${BUNDLE_ID} --asc-provider ${ASCPROVIDER} --username ${USERNAME} --password ${PASSWORD}