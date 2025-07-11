export interface SupportCard {
  id: string;
  name: string;
  icon: string; // 画像パス
  // 本来はここに得意率や初期絆などのパラメータが入る
}

export interface TrainingSlot {
  id: 'speed' | 'stamina' | 'power' | 'guts' | 'wisdom';
  name: string;
  card: SupportCard | null; // 配置されているカード
}