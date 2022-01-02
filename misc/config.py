import os
import sys
from typing import Optional, List

import yaml

class Config:
    def __init__(self, path: str):
        self._path = path
        self._h = {}
        with open(self._path, 'r') as f:
            result = yaml.safe_load(f)
            if result is not None:
                self._h = result

    @property
    def window_geometry(self) -> Optional[str]:
        return self._get('window_geometry')

    @window_geometry.setter
    def window_geometry(self, value: str) -> None:
        self._set('window_geometry', value)

    @property
    def window_state(self) -> Optional[str]:
        return self._get('window_state')

    @window_state.setter
    def window_state(self, value: str):
        self._set('window_state', value)

    @property
    def main_splitter_sizes(self) -> Optional[List[int]]:
        return self._get('main_splitter_sizes')

    @main_splitter_sizes.setter
    def main_splitter_sizes(self, value: List[int]):
        self._set('main_splitter_sizes', value)

    @property
    def secondary_splitter_sizes(self) -> Optional[List[int]]:
        return self._get('secondary_splitter_sizes')

    @secondary_splitter_sizes.setter
    def secondary_splitter_sizes(self, value: List[int]):
        self._set('secondary_splitter_sizes', value)

    @property
    def project(self) -> Optional[str]:
        return self._get('project')

    def sync(self) -> None:
        with open(self._path, 'w') as f:
            yaml.dump(self._h, f)

    def _get(self, key):
        return self._h.get(key)

    def _set(self, key, value):
        self._h[key] = value
