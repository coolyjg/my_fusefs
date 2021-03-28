extern crate cc;

fn main(){
    cc::Build::new()
        .file("src/cnt_store.c")
        .compile("cnt_store.a");
    println!("cargo:rustc-link-lib=cnt_sotre");
    println!("cargo:rustc-link-search=/home/yanjuguang/my_fusefs/src");
}

// fn main(){
//     cc::Build::new()
//         .file("src/libar.a");
// }
// extern crate dunce;
// use std::{env, path::PathBuf};

// fn main(){
    // let library_name = "test";
    // let root = PathBuf::from(env::var_os("CARGO_MANIFEST_DIR").unwrap());
    // let library_dir = dunce::canonicalize(root.join("src")).unwrap();
    // println!("cargo:rustc-link-lib=static=c{}", library_name);
    // println!("cargo:rustc-link-search=native={}", env::join_paths(&[library_dir]).unwrap().to_str().unwrap());
//     println!("cargo:rustc-link-lib=test");
//     println!("cargo:rustc-link-search=/home/yanjuguang/PMDK_development/array_store/store_cnt/src");
// }
