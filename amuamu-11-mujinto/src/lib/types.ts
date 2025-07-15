export interface CardProperties {
  specialtyRate?: number;
  friendshipBonus?: number;
  motivationBonus?: number;
  trainingBonus?: number;
  hintLevel?: number;
  hintRate?: number;
  initialBond?: number;
  speedBonus?: number;
  staminaBonus?: number;
  powerBonus?: number;
  gutsBonus?: number;
  wisdomBonus?: number;
  skillPointBonus?: number;
  raceBonus?: number;
  fanBonus?: number;
  wisdomRecovery?: number;
  failureRateDown?: number;
  energyDown?: number;
}

// カードの状態を '通常' と '固有' に限定
export type SupportCardStatus = 'normal' | 'unique';

// support-cards.ts に記述する、カードの静的な定義データ
export interface SupportCardData {
  id: string;
  name: string;
  icon?: string;
  baseProperties: Partial<CardProperties>;
  // 固有状態の時の「差分」プロパティ
  uniqueModifier?: Partial<CardProperties>;
}

// UI上で実際に扱う、動的なカードインスタンス
export interface ActiveSupportCard {
  data: SupportCardData; // 元となる静的データ
  status: SupportCardStatus; // 現在の状態
  finalProperties: Partial<CardProperties>; // 現在の状態に基づいて計算済みの最終プロパティ
}

export interface TrainingSlot {
  id: 'speed' | 'stamina' | 'power' | 'guts' | 'wisdom';
  name: string;
  cards: ActiveSupportCard[]; // 配置されているカード
}

export interface CropPreset {
  name: string;
  x: number;
  y: number;
  width: number;
  height: number;
}