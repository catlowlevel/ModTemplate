#! /usr/bin/python
import xml.etree.ElementTree as ET
import os
import re
from sys import argv
import shutil

ET.register_namespace("android", "http://schemas.android.com/apk/res/android")

mode = "debug"
do_clean_up = True


def get_main_activity_name(manifest_file):
    try:
        # Define the Android namespace
        android_ns = {"android": "http://schemas.android.com/apk/res/android"}

        # Parse the AndroidManifest.xml file
        tree = ET.parse(manifest_file)
        root = tree.getroot()

        # Find the main activity by inspecting the provided activity tag
        main_activity_name = root.find(
            ".//activity[@android:name]", namespaces=android_ns
        )
        if main_activity_name is not None:
            return main_activity_name.attrib[
                "{http://schemas.android.com/apk/res/android}name"
            ]

    except ET.ParseError as e:
        print(f"Error parsing AndroidManifest.xml: {str(e)}")
    except Exception as e:
        print(f"Error: {str(e)}")

    return None


def get_activity_path(target_path: str, activity_name: str):
    # Iterate over files in the directory
    for root, _, files in os.walk(target_path):
        for file in files:
            # Check if the file name matches the last part of the main activity name
            expected_path = "/".join(activity_name.split(".")) + ".smali"
            result = os.path.join(root, file)

            if result.endswith(expected_path):
                # Print the full path to the matching file
                return result
    return None


def update_manifest_file(manifest_file: str):
    # Update the AndroidManifest.xml file with the main activity name
    tree = ET.parse(manifest_file)
    root = tree.getroot()
    # first_permission = root.find(".//permission", namespaces=android_ns)
    # if first_permission is None:
    # print("first_permission is None")
    # return
    new_permission = ET.Element(
        "uses-permission", {"android:name": "android.permission.SYSTEM_ALERT_WINDOW"}
    )
    root.insert(0, new_permission)

    # Create a new <service> element with the specified attributes
    new_service = ET.Element(
        "service",
        {
            "android:name": "com.android.support.Launcher",
            "android:enabled": "true",
            "android:exported": "false",
            "android:stopWithTask": "true",
        },
    )

    application_tag = root.find(".//application")
    if application_tag is not None:
        application_tag.insert(len(application_tag), new_service)

    tree.write(manifest_file)


def update_activity_file(main_activity: str):
    with open(main_activity) as f:
        content = f.read()
        pattern = r"\.method.+onCreate.+\n+.+"
        matched = re.search(pattern, content)
        if matched:
            on_create = matched.group()
            new_content = f"{on_create}\n\t{'invoke-static {p0}, Lcom/android/support/Main;->Start(Landroid/content/Context;)V'}"
            content = content.replace(on_create, new_content)
            with open(main_activity, "w") as f:
                f.write(content)


def get_filename(apk: str):
    return apk.split(".")[0]


def decompile_apk(apk: str):
    # get filename
    filename = get_filename(apk)

    if os.path.exists(filename):
        shutil.rmtree(filename)

    # run a command shell
    print(f"Decompiling {apk}")
    code = os.system(f"apktool d {apk}")
    if code != 0:
        print(f"apktool d {apk} failed")
        exit(1)
    print(f"Decompiling {apk} done")
    return filename


def recompile(apk: str):
    filename = get_filename(apk)
    print(f"Recompiling {filename}")
    code = os.system(f"apktool b {filename}")
    if code != 0:
        print(f"apktool b {filename} failed")
        exit(1)
    return f"{filename}/dist/{filename}.apk"


def main(argv: list[str]):
    if len(argv) < 2:
        print("Usage: python3 build.py <.apk>")
        exit(1)

    arch = "arm64"
    selected_arch = "arm64-v8a"
    global mode
    global do_clean_up
    if "--release" in argv:
        argv.remove("--release")
        mode = "release"
        print("Release mode")
    if "--keep" in argv:
        argv.remove("--keep")
        do_clean_up = False
    if "--arm" in argv:
        argv.remove("--arm")
        arch = "arm"

    apk = argv[1]
    if arch == "arm":
        selected_arch = "armeabi-v7a"
        print("Arm mode")
    elif arch == "arm64":
        print("Arm64 mode")

    if not apk.endswith(".apk"):
        print("Usage: python3 build.py <.apk>")
        exit(1)

    mod_menu_dir = "ModMenu"

    for dirs in os.listdir():
        if dirs.endswith("ModMenu"):
            mod_menu_dir = dirs

    if mod_menu_dir == "ModMenu":
        print("ModMenu dir not found")
        exit(1)

    menu_apk = f"{mod_menu_dir}/app/build/intermediates/apk/{mode}/app-{mode}.apk"
    menu_apk2 = f"{mod_menu_dir}/app/build/outputs/apk/{mode}/app-{mode}.apk"

    # check if menu_apk exists
    if not os.path.exists(menu_apk):
        if not os.path.exists(menu_apk2):
            print("Menu apk not found")
            exit(1)
        else:
            menu_apk = menu_apk2

    # copy
    shutil.copy(menu_apk, f"menu_{get_filename(apk)}.apk")

    menu_dir = decompile_apk(f"menu_{get_filename(apk)}.apk")
    print(f"Decompiled to {menu_dir}")

    # =======================

    target_dir = decompile_apk(apk)
    print(f"Decompiled to {target_dir}")

    manifest_file = f"{target_dir}/AndroidManifest.xml"

    print(f"Looking for main activity in {manifest_file}")
    main_activity = get_main_activity_name(manifest_file)
    if not main_activity:
        print("Main activity not found in AndroidManifest.xml")
        exit(1)
    print(f"Main activity found: {main_activity}")

    print(f"Updating AndroidManifest.xml")
    update_manifest_file(manifest_file)
    print("Done")

    smali_dirs = []
    for dir in sorted(os.listdir(target_dir)):
        if "smali" in dir:
            smali_dirs.append(dir)
    last_smali = smali_dirs[-1]

    print(f"Looking for activity file in {target_dir}")
    activity_file = get_activity_path(target_dir, main_activity)
    if activity_file is None:
        print("Activity file not found")
        exit(1)
    print(f"Activity file found: {activity_file}")

    print(f"Updating activity file")
    update_activity_file(activity_file)
    print("Done")

    # copy
    # src = f"{menu_dir}/smali/com" #copies the content of com, excluding the com
    src = f"{menu_dir}/smali"
    dst = f"{target_dir}/{last_smali}"
    print(f"Copying {src} to {dst}")
    shutil.copytree(src, dst, dirs_exist_ok=True)
    src = f"{menu_dir}/lib/{selected_arch}/libMyLibName.so"
    dst = f"{target_dir}/lib/{selected_arch}/"
    print(f"Copying {src} to {dst}")
    shutil.copy(src, dst)

    # delete other arch inside lib beside selected_arch
    other_arch = os.listdir(f"{target_dir}/lib")
    other_arch = [arch for arch in other_arch if arch != selected_arch]

    for arch in other_arch:
        print(f"Deleting {target_dir}/lib/{arch}")
        shutil.rmtree(f"{target_dir}/lib/{arch}")

    result = recompile(apk)
    shutil.copy(result, f"{target_dir}_menued-{mode}.apk")
    print(f"Finished {target_dir}_menued-{mode}.apk")
    if do_clean_up:
        print("Cleaning up...")
        shutil.rmtree(target_dir)
        shutil.rmtree(menu_dir)
        os.remove(f"menu_{get_filename(apk)}.apk")
    print("Done")


print("Starting...")

if __name__ == "__main__":
    main(argv)
else:
    print("Not main")
