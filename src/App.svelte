<script lang="ts">
    import { marked } from 'marked';

    const usr = "You";

    type Message = {
        username: string;
        message: string;
    };

    let new_message = "";

    let message_list: Message[] = [{username: "John Dowe", message: "wassup"}];

    function addMessage() {
        message_list = [{username: usr, message: new_message}, ...message_list];
        new_message = "";
    }
</script>

<br/>
{#each message_list as msg}
    <div class="flex-container">
        <strong>{msg.username}</strong>
        {@html marked(msg.message)}
    </div>
<br/>
{/each}

<div class="flex-container">
    <textarea bind:value={new_message} placeholder="Write message..."></textarea>
    <button on:click={addMessage}>Send</button>
</div>

<style>
	.flex-container {
        display: flex;
        flex-direction: column;
        align-items: center;
    }
</style>