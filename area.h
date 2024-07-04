#ifndef AREA_H_
#define AREA_H_

// 日本の地域番号
// 本当は6桁の数字なので、100倍する
const uint16_t region_codes[56] = {
    0110, 0120, 0130, 0141, 0150, 0160, 0170, // 北海道
    0200, 0300, 0400, 0500, 0600, 0700, // 東北地方
    0800, 0900, 1000, 1100, 1200, 1300, 1400, 1900, 2000, // 関東甲信地方
    2100, 2200, 2300, 2400, // 東海地方
    1500, 1600, 1700, 1800, // 北陸地方
    2500, 2600, 2700, 2800, 2900, 3000, // 近畿地方
    3100, 3200, 3300, 3400, // 中国地方
    3600, 3700, 3800, 3900, // 四国地方
    3500, 4000, 4100, 4200, 4300, 4400, // 九州北部地方
    4500, 4601, // 九州南部・奄美地方
    4710, 4720, 4730, 4740 // 沖縄地方
};

const String region_names[56] = {
  "Wakkanai", "Asahikawa", "Abashiri", "Kushiro", "Muroran", "Sapporo", "Hakodate",
  "Aomori", "Morioka", "Sendai", "Akita", "Yamagata", "Fukushima",
  "Mito", "Utsunomiya", "Maebashi", "Kumagaya", "Choshi", "Tokyo(Meteorological Agency)", "Yokohama", "Kofu", "Nagano"
  "Gifu", "Shizuoka", "Nagoya", "Tsu",
  "Niigata", "Toyama", "Kanazawa", "Fukui",
  "Hikone", "Kyoto", "Osaka", "Kobe", "Nara", "Wakayama",
  "Tottori", "Matsue", "Okayama", "Hiroshima",
  "Tokushima", "Takamatsu", "Matsuyama", "Kochi",
  "Shimonoseki", "Fukuoka", "Saga", "Nagasaki", "Kumamoto", "Oita",
  "Miyazaki", "Kagoshima",
  "Okinawa", "Minamidaitou island", "Miyakojima", "Ishigakijima"
};

#endif
