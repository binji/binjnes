class Processor extends AudioWorkletProcessor {
    constructor(options) {
        super(options)

        this.waiting = true;
        this.srcOffset = 0;
        this.queue = [];
        this.minQueueLength = 2;
        this.maxQueueLength = 3;
        this.port.onmessage = this.onmessage.bind(this);
    }

    onmessage(e) {
        const buffer = e.data;
        if (this.queue.length > this.maxQueueLength) {
            console.log(`audio overflow, dropping buffer`);
            this.queue.shift();
            this.srcOffset = 0;
        }
        this.queue.push(buffer);
    }

    process(inputs, outputs, params) {
        if (this.waiting) {
            if (this.queue.length < this.minQueueLength) {
                return true;
            }
            this.waiting = false;
            console.log('done waiting');
        }
        const output = outputs[0];
        const dstLength = output[0].length;
        let dstOffset = 0;
        while (this.queue.length > 0 && dstOffset < dstLength) {
            const source = this.queue[0];
            const frames = Math.min(dstLength - dstOffset, source.length - this.srcOffset);
            output[0].set(source.subarray(this.srcOffset, this.srcOffset + frames), dstOffset);
            dstOffset += frames;
            this.srcOffset += frames;
            if (this.srcOffset === source.length) {
                this.queue.shift();
                this.srcOffset = 0;
            }
        }
        if (dstOffset < dstLength) {
            this.waiting = true;
            console.log(`audio underflow ${dstOffset} < ${dstLength}, waiting min=${this.minQueueLength} max=${this.maxQueueLength}`);
        }
        return true;
    }
}

registerProcessor('processor', Processor);
