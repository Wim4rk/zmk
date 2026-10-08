# SPDX-License-Identifier: MIT

board_runner_args(openocd "--config=${CMAKE_CURRENT_LIST_DIR}/support/openocd.cfg")

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
