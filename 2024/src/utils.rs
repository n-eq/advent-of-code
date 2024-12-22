pub(crate) fn get_test_input(module_path: &str) -> String {
    let module_name = module_path.split("::").last().unwrap();
    format!("src/{}/input_test", module_name)
}

macro_rules! input {
    // use argv[1] if provided or d{x}/input_test file
    () => {
        std::env::args()
            .collect::<Vec<String>>()
            .get(1)
            .map_or(crate::utils::get_test_input(module_path!()), |v| v.clone())
    };
}

pub(crate) use input;
