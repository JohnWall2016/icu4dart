import 'dart-ext:collator_extension';

int _createInstance(String locale) native 'createInstance';
int _compare(int instance, String first, String second) native 'compare';
void _close(int instance) native 'close';

class Collator {
  int _instance;

  Collator(String locale) {
    _instance = _createInstance(locale);
  }

  void close() => _close(_instance);

  int compare(String first, String second) =>
      _compare(_instance, first, second);
}
