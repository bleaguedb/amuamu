<script lang="ts">
  import type { SupportCard, TrainingSlot } from '$lib/types';
  import { browser } from '$app/environment';

  // (他の<script>部分のロジックは変更なし)
  let availableCards = $state<SupportCard[]>([
    { id: 'kitasan', name: 'キタサンブラック', icon: '' },
    { id: 'tazuna', name: '駿川たづな', icon: '' },
    { id: 'creek', name: 'スーパークリーク', icon: '' },
    { id: 'fine', name: 'ファインモーション', icon: '' },
  ]);
  interface UpgradedTrainingSlot {
    id: 'speed' | 'stamina' | 'power' | 'guts' | 'wisdom';
    name: string;
    cards: SupportCard[];
  }
  let trainingSlots = $state<UpgradedTrainingSlot[]>([
    { id: 'speed', name: 'スピード練習', cards: [] },
    { id: 'stamina', name: 'スタミナ練習', cards: [] },
    { id: 'power', name: 'パワー練習', cards: [] },
    { id: 'guts', name: '根性練習', cards: [] },
    { id: 'wisdom', name: '賢さ練習', cards: [] },
  ]);

  let dragOverSlotId = $state<string | null>(null);

  function handleDragStart(event: DragEvent, cardId: string) {
    event.dataTransfer?.setData('text/plain', cardId);
  }

  // 💡 提案2: カードを探して削除するロジックをヘルパー関数に集約
  function findAndRemoveCard(cardId: string): SupportCard | undefined {
    // availableCardsから探す
    const fromAvailableIndex = availableCards.findIndex((c) => c.id === cardId);
    if (fromAvailableIndex > -1) {
      return availableCards.splice(fromAvailableIndex, 1)[0];
    }
    // trainingSlotsから探す
    for (const slot of trainingSlots) {
      const fromSlotIndex = slot.cards.findIndex((c) => c.id === cardId);
      if (fromSlotIndex > -1) {
        return slot.cards.splice(fromSlotIndex, 1)[0];
      }
    }
    return undefined;
  }

  function handleDrop(event: DragEvent, targetSlotId: string) {
    event.preventDefault();
    dragOverSlotId = null; // drop時にもリセット
    const cardId = event.dataTransfer?.getData('text/plain');
    if (!cardId) return;

    const targetSlot = trainingSlots.find((s) => s.id === targetSlotId);
    if (!targetSlot || targetSlot.cards.length >= 5) {
      return;
    }

    const droppedCard = findAndRemoveCard(cardId);
    if (droppedCard) {
      targetSlot.cards.push(droppedCard);
    }
  }
</script>

{#if browser}
  <div class="container mx-auto p-4">
    <h1 class="h1 mb-4">ウマ娘 トレーニングシミュレーター</h1>

    <div class="grid grid-cols-1 lg:grid-cols-3 gap-4">
      <div class="card col-span-1">
        <h2 class="h2">サポートカード</h2>
        <div class="flex flex-col gap-2">
          {#each availableCards as card (card.id)}
            <div
              role="button"
              tabindex="0"
              aria-label="{card.name}をドラッグ"
              class="card p-2 flex items-center gap-2 cursor-grab variant-soft-surface hover:variant-soft-primary"
              draggable="true"
              ondragstart={(e) => handleDragStart(e, card.id)}
            >
              <div
                class="w-10 h-10 bg-primary-500 rounded-md flex items-center justify-center text-white font-bold text-lg"
              >
                {card.name.substring(0, 1)}
              </div>
              <span class="font-bold">{card.name}</span>
            </div>
          {/each}
        </div>
      </div>

      <div class="col-span-1 lg:col-span-2 flex flex-col gap-4">
        <div class="card">
          <div class="grid grid-cols-2 md:grid-cols-5 gap-2">
            {#each trainingSlots as slot (slot.id)}
              <div
                role="group"
                aria-label="{slot.name}のドロップゾーン"
                class="card p-4 aspect-square flex flex-col items-center text-center space-y-1 transition-colors {dragOverSlotId ===
                slot.id
                  ? 'variant-soft-primary'
                  : ''}"
                ondragover={(e) => {
                  e.preventDefault(); // 💡 提案3: ハンドラをインライン化
                  dragOverSlotId = slot.id;
                }}
                ondragleave={() => (dragOverSlotId = null)}
                ondrop={(e) => handleDrop(e, slot.id)}
              >
                <h3 class="h3 mb-2">{slot.name} ({slot.cards.length}/5)</h3>
                {#each slot.cards as card (card.id)}
                  <div
                    role="button"
                    tabindex="0"
                    aria-label="{card.name}をドラッグ"
                    class="w-full text-center cursor-grab variant-soft-secondary rounded-md p-1"
                    draggable="true"
                    ondragstart={(e) => {
                      e.stopPropagation(); // イベントの伝播を停止
                      handleDragStart(e, card.id);
                    }}
                  >
                    <p class="text-sm font-bold">{card.name}</p>
                  </div>
                {/each}
                {#if slot.cards.length === 0}
                  <div class="flex-1 flex items-center justify-center">
                    <p class="text-surface-500">ここに配置</p>
                  </div>
                {/if}
              </div>
            {/each}
          </div>
        </div>
      </div>
    </div>
  </div>
{/if}