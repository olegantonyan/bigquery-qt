import os
import sys
from typing import Optional, List

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

    @property
    def main_splitter_sizes(self) -> Optional[List[int]]:
        return self._h.get('main_splitter_sizes')

    @main_splitter_sizes.setter
    def main_splitter_sizes(self, value: List[int]):
        self._h['main_splitter_sizes'] = value

    @property
    def secondary_splitter_sizes(self) -> Optional[List[int]]:
        return self._h.get('secondary_splitter_sizes')

    @secondary_splitter_sizes.setter
    def secondary_splitter_sizes(self, value: List[int]):
        self._h['secondary_splitter_sizes'] = value

    @property
    def project(self) -> Optional[str]:
        return self._h.get('project')

    def sync(self):
        with open(self._path, 'w') as f:
            yaml.dump(self._h, f)
