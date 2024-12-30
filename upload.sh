filename=local.properties
thekey="IS_UPLOAD_TO_MAVEN"
newvalue="true"

if ! grep -R "^[#]*\s*${thekey}=.*" $filename > /dev/null; then
  echo "APPENDING because '${thekey}' not found"
  echo "$thekey=$newvalue" >> $filename
else
  echo "SETTING because '${thekey}' found already"
  sed -ir "s/^[#]*\s*${thekey}=.*/$thekey=$newvalue/" $filename
fi

./gradlew :common:upload

./gradlew :jsc:upload

./gradlew :quickjs:upload

./gradlew :v8:upload

./gradlew :v8-no-op:upload

newvalue="false"
sed -ir "s/^[#]*\s*${thekey}=.*/$thekey=$newvalue/" $filename
rm local.propertiesr
