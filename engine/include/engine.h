#ifndef STRATEGIENT_ENGINE_H
#define STRATEGIENT_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns the engine version as a C string.
 * Caller MUST call engine_free_string() to release memory.
 */
char* engine_version(void);

/**
 * Frees a string previously allocated by the engine.
 * @param ptr Pointer returned by engine functions (e.g., engine_version).
 *            Safe to call with NULL.
 */
void engine_free_string(char* ptr);

#ifdef __cplusplus
}
#endif

#endif /* STRATEGIENT_ENGINE_H */

