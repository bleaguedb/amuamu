<script lang="ts">
import type { ActiveSupportCard, SupportCardData, TrainingSlot, SupportCardStatus } from '$lib/types';
import { browser } from '$app/environment';
import { base } from '$app/paths';
import FrameCropper from '$lib/components/FrameCropper.svelte';
import { allSupportCards } from '$lib/data/support-cards';

// --- カード操作関数 ---
function createActiveCard(data: SupportCardData): ActiveSupportCard {
  const cardDataWithIcon: SupportCardData = {
    ...data,
    icon: data.icon || `${base}/images/support-cards/icon/${data.id}.png`,
  };
  return {
    data: cardDataWithIcon,
    status: 'normal',
    finalProperties: { ...cardDataWithIcon.baseProperties },
  };
}

function changeCardStatus(card: ActiveSupportCard, newStatus: SupportCardStatus): ActiveSupportCard {
  const finalProperties = { ...card.data.baseProperties };
  if (newStatus === 'unique' && card.data.uniqueModifier) {
    Object.assign(finalProperties, card.data.uniqueModifier);
  }
  return { ...card, status: newStatus, finalProperties };
}

// --- 状態管理 ---
const initialCardIds = ['pow-ardan', 'wis-daska', 'spd-amoai', 'spd-still', 'pow-tamamo', 'fri-takker'];
let availableCards = $state<ActiveSupportCard[]>(
  allSupportCards
    .filter(data => initialCardIds.includes(data.id))
    .map(createActiveCard)
);

let trainingSlots = $state<TrainingSlot[]>([
  { id: 'speed', name: 'スピ練', cards: [] },
  { id: 'stamina', name: 'スタ練', cards: [] },
  { id: 'power', name: 'パワ練', cards: [] },
  { id: 'guts', name: '根性練', cards: [] },
  { id: 'wisdom', name: '賢さ練', cards: [] },
]);

let dragOverSlotId = $state<string | null>(null);

// --- ドラッグ&ドロップ関連の関数 ---
function handleDragStart(event: DragEvent, cardId: string) {
  event.dataTransfer?.setData('text/plain', cardId);
}

function findAndRemoveCard(cardId: string): ActiveSupportCard | undefined {
  const fromAvailableIndex = availableCards.findIndex((c) => c.data.id === cardId);
  if (fromAvailableIndex > -1) {
    return availableCards.splice(fromAvailableIndex, 1)[0];
  }
  for (const slot of trainingSlots) {
    const fromSlotIndex = slot.cards.findIndex((c) => c.data.id === cardId);
    if (fromSlotIndex > -1) {
      return slot.cards.splice(fromSlotIndex, 1)[0];
    }
  }
  return undefined;
}

function handleDrop(event: DragEvent, targetArea: string) {
  event.preventDefault();
  dragOverSlotId = null; // スロットのドラッグオーバー状態をリセット
  const cardId = event.dataTransfer?.getData('text/plain');
  if (!cardId) return;

  const droppedCard = findAndRemoveCard(cardId);
  if (!droppedCard) return;

  if (targetArea === 'available') {
    availableCards.push(droppedCard);
  } else {
    const targetSlot = trainingSlots.find((s) => s.id === targetArea);
    if (!targetSlot || targetSlot.cards.length >= 5) {
      // ドロップが無効な場合はカードを元の場所に戻す
      // findAndRemoveCardが既に削除しているので、availableCardsに戻す
      availableCards.push(droppedCard);
      return;
    }
    // 同じカードが既にスロットにある場合は移動させない
    if (targetSlot.cards.some(c => c.data.id === cardId)) {
      availableCards.push(droppedCard); // 元に戻す
      return;
    }
    targetSlot.cards.push(droppedCard);
  }
}

function toggleCardStatus(slotId: string, cardId: string) {
  const slot = trainingSlots.find(s => s.id === slotId);
  if (!slot) return;

  const cardIndex = slot.cards.findIndex(c => c.data.id === cardId);
  if (cardIndex === -1) return;

  const currentCard = slot.cards[cardIndex];
  const newStatus = currentCard.status === 'normal' ? 'unique' : 'normal';
  slot.cards[cardIndex] = changeCardStatus(currentCard, newStatus);
}

// --- 切り抜き用 ---
let stream = $state<MediaStream | null>(null);
let snapshot = $state<ImageBitmap | null>(null);
let showFrameCropper = $state(false);
let videoEl: HTMLVideoElement;
let resolution = $state<{ width: number; height: number } | null>(null);

async function handleStartCapture() {
	if (!browser) return;
	try {
		// 既存のストリームを停止
		stream?.getTracks().forEach((track) => track.stop());
		resolution = null;

		const displayStream = await navigator.mediaDevices.getDisplayMedia({ video: true });
		const videoTrack = displayStream.getVideoTracks()[0];

		if (videoTrack) {
			videoTrack.onended = () => {
				stream = null;
				resolution = null;
			};
		}

		stream = displayStream;
		videoEl.srcObject = displayStream;
	} catch (err) {
		console.log('キャプチャがキャンセルされました。');
		stream = null;
		resolution = null;
	}
}

function updateResolution() {
	if (videoEl && videoEl.videoWidth > 0) {
		resolution = { width: videoEl.videoWidth, height: videoEl.videoHeight };
	}
}

async function handleTakeSnapshot() {
  if (!stream || !videoEl) {
    alert('先に「キャプチャ開始」ボタンを押してください。');
    return;
  }
  snapshot = await createImageBitmap(videoEl);
  showFrameCropper = true;
}

// onCropコールバックとして渡す関数
function handleCrop(result: { name: string; dataUrl: string }) {
  const newCardData: SupportCardData = {
    id: `custom-${Date.now()}`,
    name: result.name,
    icon: result.dataUrl,
    baseProperties: {},
  };
  const newActiveCard = createActiveCard(newCardData);
  availableCards.push(newActiveCard);
  showFrameCropper = false;
}
</script>

<video
  bind:this={videoEl}
  class="hidden"
  autoplay
  playsinline
  onloadedmetadata={updateResolution}
  onresize={updateResolution}
><track kind="captions" /></video>

{#if browser}
  {#if showFrameCropper && snapshot}
    <div class="fixed inset-0 bg-surface-900/75 flex items-center justify-center z-50 p-4">
      <FrameCropper
        snapshot={snapshot}
				close={() => showFrameCropper = false}
      />
    </div>
  {/if}

  <div class="container mx-auto p-4">
    <h1 class="h1 mb-4">ウマ娘 トレーニングシミュレーター</h1>

    <div class="grid grid-cols-1 lg:grid-cols-3 gap-4">
      <div class="lg:col-span-1 flex flex-col gap-4">
        <div class="card p-4">
          <h2 class="h2 mb-4">操作パネル</h2>
          {#if !stream}
            <div class="flex justify-center">
              <button type="button" class="btn preset-outlined-primary-500 h-12 px-8" onclick={handleStartCapture}>
                <span class="text-lg">キャプチャ開始</span>
              </button>
            </div>
          {:else}
            <div class="text-center">
              <p class="text-success-500 font-bold mb-2 animate-pulse">● キャプチャ中</p>
              {#if resolution}
                <p class="text-sm mb-2">解像度: {resolution.width} x {resolution.height}</p>
              {/if}
              <button class="btn preset-outlined-primary-500 h-16 px-8" onclick={handleTakeSnapshot}>
                <span class="text-lg">切り抜き実行</span>
              </button>
            </div>
          {/if}
        </div>

        <div class="card p-4"
          ondragover={(e) => {
            e.preventDefault();
            dragOverSlotId = 'available';
          }}
          ondragleave={() => (dragOverSlotId = null)}
          ondrop={(e) => handleDrop(e, 'available')}
        >
          <h2 class="h2 mb-4">サポートカード</h2>
          <div class="flex flex-col gap-2">
            {#each availableCards as card (card.data.id)}
              <div
                role="button"
                tabindex="0"
                aria-label="{card.data.name}をドラッグ"
                class="card p-2 flex items-center gap-2 cursor-grab preset-tonal-surface hover:preset-tonal-primary"
                draggable="true"
                ondragstart={(e) => handleDragStart(e, card.data.id)}
              >
                {#if card.data.icon}
                  <img src={card.data.icon} alt={card.data.name} class="w-10 h-10 rounded-md object-cover bg-surface-500" />
                {:else}
                  <div class="w-10 h-10 bg-primary-500 rounded-md flex items-center justify-center text-white font-bold text-lg">
                    {card.data.name.substring(0, 1)}
                  </div>
                {/if}
                <span class="font-bold">{card.data.name}</span>
              </div>
            {/each}
          </div>
        </div>
      </div>

      <div class="lg:col-span-2 card p-4">
        <div class="grid grid-cols-2 md:grid-cols-3 lg:grid-cols-5 gap-2">
          {#each trainingSlots as slot (slot.id)}
            <div
              role="group"
              aria-label="{slot.name}のドロップゾーン"
              class="card p-4 min-h-[320px] flex flex-col items-center text-center gap-y-1 transition-colors {dragOverSlotId === slot.id ? 'preset-tonal-primary' : ''}"
              ondragover={(e) => {
                e.preventDefault();
                dragOverSlotId = slot.id;
              }}
              ondragleave={() => (dragOverSlotId = null)}
              ondrop={(e) => handleDrop(e, slot.id)}
            >
              <h3 class="h3 mb-2 w-full">{slot.name}<br>({slot.cards.length}/5)</h3>
              {#if slot.cards.length === 0}
                <div class="flex-1 flex items-center justify-center">
                  <p class="text-surface-500">ここに配置</p>
                </div>
              {:else}
                <div class="flex-1 w-full flex flex-col gap-y-1 overflow-y-auto">
                  {#each slot.cards as card (card.data.id)}
                    <div
                      role="button"
                      tabindex="0"
                      aria-label="{card.data.name}をドラッグ。クリックで状態切替"
                      class="w-full text-center cursor-grab rounded-md p-1 relative"
                      draggable="true"
                      ondragstart={(e) => {
                        e.stopPropagation();
                        handleDragStart(e, card.data.id);
                      }}
                      onclick={() => toggleCardStatus(slot.id, card.data.id)}
                    >
                      {#if card.status === 'unique'}
                        <div class="absolute top-0 right-0 w-4 h-4 bg-accent-500 rounded-full border-2 border-surface-100-900" title="固有ボーナス発動中"></div>
                      {/if}
                      {#if card.data.icon}
                        <img src={card.data.icon} alt={card.data.name} class="w-12 h-12 rounded-md object-cover mx-auto" />
                      {:else}
                        <div class="w-12 h-12 bg-primary-500 rounded-md flex items-center justify-center text-white font-bold text-lg mx-auto">
                          {card.data.name.substring(0, 1)}
                        </div>
                      {/if}
                    </div>
                  {/each}
                </div>
              {/if}
            </div>
          {/each}
        </div>
      </div>
    </div>
  </div>
{/if}

<style lang="postcss">
  @reference "tailwindcss";
  :global(html) {
    background-color: var(--color-surface-100-900);
  }
</style>