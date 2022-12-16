<script lang="ts">
    import { marked } from 'marked';
    import { compress } from 'lz-string';

    const usr = "You";

    type Message = {
        username: string;
        message: string;
    };

    let new_message = "";

    let message_list: Message[] = [{username: "John Dowe", message: "wassup"}];

    function addMessage() {
        let compressed = compress(new_message);
        console.log("Original message: " + new_message + " | " + new_message.length);
        console.log("Compressed message: " + compressed + " | " + compressed.length);
        message_list = [{username: usr, message: new_message}, ...message_list];
        new_message = "";
    }

    function msgSubmitKet(e: KeyboardEvent) {
        if (e.shiftKey && e.key == 'Enter') {
            addMessage();
        }
    }
    function submitClear(e: KeyboardEvent) {
        if (e.shiftKey && e.key == 'Enter') {
            new_message = "";
        }
    }
</script>

<br/>
{#each message_list as msg}
    <div class="message">
        <strong>{msg.username}</strong>
        {@html marked(msg.message)}
    </div>
<br/>
{/each}

<div class="flex-container footer">
    <textarea on:keydown={msgSubmitKet} on:keyup={submitClear} bind:value={new_message} placeholder="Write message..."></textarea>
    <button on:click={addMessage}>Send</button>
</div>

<style>
    .message {
        display: flex;
        flex-direction: column;
        align-items: center;
        box-shadow: 4px 6px #888888;
        border-radius: 8px;
        outline: auto;
    }
	.flex-container {
        display: flex;
        flex-direction: column;
        align-items: center;
    }
    .footer {
        position: fixed;
        bottom: 0;
        width: 100%;
    }
</style>