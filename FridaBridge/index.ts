import "frida-il2cpp-bridge";

Il2Cpp.perform(() => {
    console.log(`Unity Version ${Il2Cpp.unityVersion}`);
    const domain = Il2Cpp.domain;

    Il2Cpp.dump();

    // Il2Cpp.trace()
    //     .assemblies(domain.assembly("Assembly-CSharp"))
    //     .and()
    //     .attach();
});
