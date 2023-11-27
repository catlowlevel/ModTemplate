# ModTemplate
My Template for modding unity(il2cpp) games for android

## Getting Started
Clone the repository
```console
git clone --depth=1 https://github.com/catlowlevel/ModTemplate.git ProjectName
```
Build the ModMenu
```console
cd ProjectName/TemplateModMenu
gradle assembleDebug
```
add the mod menu to the game apk
```console
cd ../
<copy game apk to current dir>
./build.py game.apk
```
