import type { SupportCardData } from '$lib/types';

export const allSupportCards: SupportCardData[] = [
  {
    id: 'pow-ardan',
    name: 'パワアルダン',
    baseProperties: {
      friendshipBonus: 40,
      motivationBonus: 30,
      trainingBonus: 10,
      initialBond: 25,
      skillPointBonus: 2,
      hintLevel: 2,
      hintRate: 30,
      raceBonus: 5,
      fanBonus: 10,
    },
    uniqueModifier: {
      staminaBonus: 1,
      powerBonus: 1,
    },
  },
  {
    id: 'wis-daska',
    name: '賢さダスカ',
    baseProperties: {},
  },
  {
    id: 'spd-amoai',
    name: 'スピードアイ',
    baseProperties: {},
  },
  {
    id: 'spd-still',
    name: 'スピードスティル',
    baseProperties: {},
  },
  {
    id: 'pow-tamamo',
    name: 'パワータマモ',
    baseProperties: {},
  },
  {
    id: 'fri-takker',
    name: '友人タッカー',
    baseProperties: {},
  },
];
