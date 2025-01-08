#[macro_export]
macro_rules! input {
    // use argv[1] if provided or use the sample from inputs/samples
    () => {
        std::env::args()
            .collect::<Vec<String>>()
            .get(1)
            .map_or(crate::get_test_input(module_path!()), |v| v.clone())
    };
}

pub fn get_test_input(module_path: &str) -> String {
    let module_name = module_path.split("::").last().unwrap();
    format!(
        "{}/inputs/samples/{}",
        std::env::current_dir().unwrap().display(),
        module_name
    )
}
