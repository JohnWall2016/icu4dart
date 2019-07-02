import 'package:icu4dart/icu4dart.dart';

main() {
  var collator = Collator('zh');
  var list = [
    "长城乡",
    "昭潭街道",
    "先锋街道",
    "万楼街道",
    "楠竹山镇",
    "姜畲镇",
    "鹤岭镇",
    "城正街街道",
    "雨湖路街道",
    "云塘街道",
    "窑湾街道",
    "广场街道"
  ];
  print(list);
  list.sort((first, second) => collator.compare(first, second));
  print(list);
  collator.close();
}
