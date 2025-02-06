fn main() {
    cxx_build::bridge("src/lib.rs")
        .file("src/wrap.cc")
        .flag_if_supported("-std=c++14")
        .compile("cxxbridge-kahypar");
    println!("cargo::rustc-link-search=/usr/local/lib");
    println!("cargo::rustc-link-lib=kahypar");
    // println!("cargo::rustc-link-lib=mtkahypar");
    println!("cargo:rerun-if-changed=src/wrap.cc");
}
