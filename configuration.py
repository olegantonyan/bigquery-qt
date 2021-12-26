import os
import sys
from typing import Optional

import yaml

class Configuration:
    def __init__(self, path: str):
        self._path = path
        self._h = {}
        with open(self._path, 'r') as f:
            result = yaml.safe_load(f)
            if result is not None:
                self._h = result

    @property
    def window_geometry(self) -> Optional[str]:
        return self._h.get('window_geometry')

    @window_geometry.setter
    def window_geometry(self, value: str):
        self._h['window_geometry'] = value

    @property
    def window_state(self) -> Optional[str]:
        return self._h.get('window_state')

    @window_state.setter
    def window_state(self, value: str):
        self._h['window_state'] = value

    def sync(self):
        with open(self._path, 'w') as f:
            yaml.dump(self._h, f)
