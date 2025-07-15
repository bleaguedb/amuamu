<script lang="ts">
	import { browser } from '$app/environment';
import type { CropPreset } from '$lib/types';

// props
let { snapshot, close } = $props<{
		snapshot: ImageBitmap | null,
		close: () => void;
	}>();

// 状態管理
let canvasEl = $state<HTMLCanvasElement>();
let captureName = $state('');
let cropX = $state(100);
let cropY = $state(100);
let cropWidth = $state(150);
let cropHeight = $state(150);
let originalImage: ImageData | null = null;
let isDragging = $state(false);
let dragStartOffset = $state({ x: 0, y: 0 });

const presets: CropPreset[] = [
	{ name: '練習サポ1', x: 445, y: 172, width: 72, height: 78 },
	{ name: 'プリセット2', x: 200, y: 200, width: 120, height: 120 },
	{ name: 'プリセット3', x: 350, y: 350, width: 80, height: 80 }
];

// --- 関数定義 ---
function applyPreset(preset: CropPreset) {
	cropX = preset.x;
	cropY = preset.y;
	cropWidth = preset.width;
	cropHeight = preset.height;
}

function saveCrop() {
		if (!canvasEl || !captureName.trim()) return;
		const tempCanvas = document.createElement('canvas');
		tempCanvas.width = cropWidth;
		tempCanvas.height = cropHeight;
		const tempCtx = tempCanvas.getContext('2d');
		if (tempCtx) {
			tempCtx.drawImage(canvasEl, cropX, cropY, cropWidth, cropHeight, 0, 0, cropWidth, cropHeight);
			const dataUrl = tempCanvas.toDataURL('image/png');
			const link = document.createElement('a');
			link.href = dataUrl;
			link.download = `${captureName}.png`;
			document.body.appendChild(link);
			link.click();
			document.body.removeChild(link);
		}
	}

	function handleMouseDown(event: MouseEvent) {
		if (!canvasEl) return;
		const rect = canvasEl.getBoundingClientRect();
		const scaleX = canvasEl.width / rect.width;
		const scaleY = canvasEl.height / rect.height;
		const mouseX = (event.clientX - rect.left) * scaleX;
		const mouseY = (event.clientY - rect.top) * scaleY;

		if (mouseX >= cropX && mouseX <= cropX + cropWidth && mouseY >= cropY && mouseY <= cropY + cropHeight) {
			isDragging = true;
			dragStartOffset = { x: mouseX - cropX, y: mouseY - cropY };
		}
	}

	function handleMouseMove(event: MouseEvent) {
		if (!isDragging || !canvasEl) return;
		const rect = canvasEl.getBoundingClientRect();
		const scaleX = canvasEl.width / rect.width;
		const scaleY = canvasEl.height / rect.height;
		const mouseX = (event.clientX - rect.left) * scaleX;
		const mouseY = (event.clientY - rect.top) * scaleY;
		
		cropX = Math.round(mouseX - dragStartOffset.x);
		cropY = Math.round(mouseY - dragStartOffset.y);
	}

	function handleMouseUp() {
		isDragging = false;
	}

	function handleMouseLeave() {
		isDragging = false;
	}

	// --- Effects --
  $effect(() => {
		if (snapshot && canvasEl) {
			canvasEl.width = snapshot.width;
			canvasEl.height = snapshot.height;
			const ctx = canvasEl.getContext('2d', { willReadFrequently: true });
			if (!ctx) return;
			
			// ImageBitmapをdrawImageで描画
			ctx.drawImage(snapshot, 0, 0);
			
			// 再描画用にImageDataとして保持
			originalImage = ctx.getImageData(0, 0, snapshot.width, snapshot.height);

			// 最初の切り抜き枠を描画
			ctx.strokeStyle = 'rgba(255, 0, 0, 0.9)';
			ctx.lineWidth = 3;
			ctx.strokeRect(cropX, cropY, cropWidth, cropHeight);
		}
	});
  
	$effect(() => {
		if (!canvasEl || !originalImage || !isDragging) return;
		const ctx = canvasEl.getContext('2d', { willReadFrequently: true });
		if (!ctx) return;
		ctx.putImageData(originalImage, 0, 0);
		ctx.strokeStyle = 'rgba(255, 0, 0, 0.9)';
		ctx.lineWidth = 3;
		ctx.strokeRect(cropX, cropY, cropWidth, cropHeight);
	});
</script>

<div class="card p-4 sm:p-6 w-full max-w-7xl mx-auto text-white" role="dialog" aria-modal="true">
	<h2 class="h2 mb-4">フレームキャプチャ & 切り抜き</h2>
	<div class="flex flex-col md:flex-row gap-6">
		<div class="flex-auto md:w-3/4">
			<div class="relative bg-black rounded-md ring-1 ring-surface-500/50 overflow-hidden flex justify-center items-center h-full">
				<canvas
					bind:this={canvasEl}
					class="max-w-full max-h-[75vh]"
					class:cursor-grab={!isDragging}
					class:cursor-grabbing={isDragging}
					onmousedown={handleMouseDown}
					onmousemove={handleMouseMove}
					onmouseup={handleMouseUp}
					onmouseleave={handleMouseLeave}
				></canvas>
			</div>
		</div>
		<div class="flex-shrink-0 md:w-1/4 flex flex-col">
			<div class="variant-soft-surface p-4 rounded-md space-y-4 flex-grow ring-1 ring-surface-500/30">
				<div>
					<h3 class="h3 mb-2">切り抜き座標</h3>
					<div class="grid grid-cols-2 gap-x-4 gap-y-2">
						<label class="label"><span>X座標</span><input class="input placeholder:text-surface-400" type="number" bind:value={cropX} /></label>
						<label class="label"><span>Y座標</span><input class="input placeholder:text-surface-400" type="number" bind:value={cropY} /></label>
						<label class="label"><span>幅</span><input class="input placeholder:text-surface-400" type="number" bind:value={cropWidth} /></label>
						<label class="label"><span>高さ</span><input class="input placeholder:text-surface-400" type="number" bind:value={cropHeight} /></label>
					</div>
					<div class="mt-4">
						<h4 class="h4 mb-2">プリセット</h4>
						<div class="flex flex-wrap gap-2">
							{#each presets as preset}
								<button class="btn preset-tonal-primary-500 text-sm py-1 px-3" onclick={() => applyPreset(preset)}>
									{preset.name}
								</button>
							{/each}
						</div>
					</div>
				</div>
				<div>
					<label class="label">
						<span>名前</span>
						<input class="input placeholder:text-surface-400" type="text" bind:value={captureName} placeholder="識別するための名前" />
					</label>
				</div>
			</div>
			<div class="flex gap-2 mt-4">
				<button class="btn variant-filled-primary flex-1" onclick={saveCrop} disabled={!captureName.trim()}>
					保存
				</button>
				<button class="btn variant-ghost-surface" onclick={close}>キャンセル</button>
			</div>
		</div>
	</div>
</div>