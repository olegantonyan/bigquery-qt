from PySide6.QtCore import QByteArray


def qbytearray_to_str(arg: QByteArray) -> str:
    return str(arg.toHex(), 'utf-8')

def qbytearray_from_str(arg: str) -> QByteArray:
    return QByteArray.fromHex(QByteArray(arg))
