void handle_error(int return_value, char *filename);
void save_benchmark_result(long time, char *filename);
void save_benchmark_result_float(float time, char *filename);
void save_benchmark_result_multiple(long time1, long time2, char *filename);
void save_benchmark_result_partial(float time, int index, char *prefix);