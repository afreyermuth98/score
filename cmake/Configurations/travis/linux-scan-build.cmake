set(CMAKE_BUILD_TYPE Release CACHE INTERNAL "")

set(DISABLE_COTIRE True CACHE BOOL "No cotire" FORCE)
set(OSSIA_SANITIZE True CACHE BOOL "Sanitize ossia" FORCE)
set(SCORE_COTIRE False)
set(DEPLOYMENT_BUILD False)
set(SCORE_COVERAGE False)
set(SCORE_AUDIO_PLUGINS True CACHE INTERNAL "")
set(SCORE_ENABLE_LTO False)
include(all-plugins)