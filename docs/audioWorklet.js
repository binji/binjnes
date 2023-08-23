class Processor extends AudioWorkletProcessor {
    constructor(options) {
        super(options)

        this.srcOffset = 0;
        this.queue = [];
        this.port.onmessage = this.onmessage.bind(this);
    }

    onmessage(e) {
        const buffer = e.data;
        this.queue.push(buffer);
    }

    process(inputs, outputs, params) {
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
        return true;
    }
}

registerProcessor('processor', Processor);