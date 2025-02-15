# Copyright (c) Prophesee S.A.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
# on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and limitations under the License.

"""
Sample code that demonstrates how to use Metavision SDK to visualize events from a live camera or a RAW file
"""


from metavision_core.event_io import EventsIterator, LiveReplayEventsIterator
from metavision_sdk_core import PeriodicFrameGenerationAlgorithm
from metavision_sdk_ui import EventLoop, BaseWindow, MTWindow, UIAction, UIKeyEvent
import argparse
import os


def parse_args():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description='Metavision Simple Viewer sample.',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        '-i', '--input-raw-file', dest='input_path', default="",
        help="Path to input RAW file. If not specified, the live stream of the first available camera is used. "
        "If it's a camera ID, it will try to open that camera instead.")
    args = parser.parse_args()
    return args


def is_live_camera(input_path):
    """Checks if input_path is a live camera
    Args:
        input_path (str): path to the file to read. if `path` is an empty string or a camera id,
        this function will return true.
    """
    return isinstance(input_path, str) and not os.path.exists(input_path)


def main():
    """ Main """
    args = parse_args()

    # Events iterator on Camera or RAW file
    mv_iterator = EventsIterator(input_path=args.input_path, delta_t=1e3)
    height, width = mv_iterator.get_size()  # Camera Geometry

    # Helper iterator to emulate realtime
    if not is_live_camera(args.input_path):
        mv_iterator = LiveReplayEventsIterator(mv_iterator)

    # Window - Graphical User Interface
    with MTWindow(title="Metavision Events Viewer", width=width, height=height, mode=BaseWindow.RenderMode.BGR) as window:
        def keyboard_cb(key, scancode, action, mods):
            if key == UIKeyEvent.KEY_ESCAPE or key == UIKeyEvent.KEY_Q:
                window.set_close_flag()

        window.set_keyboard_callback(keyboard_cb)

        # Event Frame Generator
        event_frame_gen = PeriodicFrameGenerationAlgorithm(sensor_width=width, sensor_height=height, fps=25)

        def on_cd_frame_cb(ts, cd_frame):
            window.show_async(cd_frame)

        event_frame_gen.set_output_callback(on_cd_frame_cb)

        # Process events
        for evs in mv_iterator:
            # Dispatch system events to the window
            EventLoop.poll_and_dispatch()
            event_frame_gen.process_events(evs)

            if window.should_close():
                break


if __name__ == "__main__":
    main()
