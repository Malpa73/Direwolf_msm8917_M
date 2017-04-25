/*
 * Copyright (C) 2017, Sultanxda <sultanxda@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt) "iosched-swch: " fmt

#include <linux/blkdev.h>
#include <linux/blk_types.h>
#include <linux/elevator.h>
#include <linux/fb.h>

#define NOOP_IOSCHED "noop"
<<<<<<< HEAD
#define RESTORE_DELAY_MS (5000)

struct req_queue_data {
	struct list_head list;
=======
#define RESTORE_DELAY_MS (10000)

struct iosched_conf {
	struct delayed_work restore_prev;
>>>>>>> 39ce106afb9a... block: Add driver to change the I/O scheduler when the screen turns off
	struct request_queue *queue;
	char prev_e[ELV_NAME_MAX];
	bool using_noop;
};

<<<<<<< HEAD
static struct delayed_work restore_prev;
static struct delayed_work sleep_sched;
static DEFINE_SPINLOCK(init_lock);
static struct req_queue_data req_queues = {
	.list = LIST_HEAD_INIT(req_queues.list),
};

static void change_elevator(struct req_queue_data *r, bool use_noop)
{
	struct request_queue *q = r->queue;

	if (r->using_noop == use_noop)
		return;

	r->using_noop = use_noop;

	if (use_noop) {
		strcpy(r->prev_e, q->elevator->type->elevator_name);
		elevator_change(q, NOOP_IOSCHED);
	} else {
		elevator_change(q, r->prev_e);
	}
}

static void change_all_elevators(struct list_head *head, bool use_noop)
{
	struct req_queue_data *r;

	list_for_each_entry(r, head, list)
		change_elevator(r, use_noop);
}

static int fb_notifier_callback(struct notifier_block *nb,
		unsigned long action, void *data)
{
=======
static struct iosched_conf *config_g;

static void change_elevator(struct iosched_conf *c, bool use_noop)
{
	struct request_queue *q = c->queue;
	char name[ELV_NAME_MAX];

	if (c->using_noop == use_noop)
		return;

	c->using_noop = use_noop;

	spin_lock_irq(q->queue_lock);
	strcpy(name, q->elevator->type->elevator_name);
	spin_unlock_irq(q->queue_lock);

	if (use_noop) {
		if (strcmp(name, NOOP_IOSCHED)) {
			strcpy(c->prev_e, name);
			elevator_change(q, NOOP_IOSCHED);
		}
	} else {
		if (!strcmp(name, NOOP_IOSCHED))
			elevator_change(q, c->prev_e);
	}
}

static int fb_notifier_callback(struct notifier_block *nb,
		unsigned long action, void *data)
{
	struct iosched_conf *c = config_g;
>>>>>>> 39ce106afb9a... block: Add driver to change the I/O scheduler when the screen turns off
	struct fb_event *evdata = data;
	int *blank = evdata->data;

	/* Parse framebuffer events as soon as they occur */
	if (action != FB_EARLY_EVENT_BLANK)
		return NOTIFY_OK;

	switch (*blank) {
	case FB_BLANK_UNBLANK:
		/*
		 * Switch back from noop to the original iosched after a delay
		 * when the screen is turned on.
		 */
<<<<<<< HEAD
		if (delayed_work_pending(&sleep_sched))
			cancel_delayed_work_sync(&sleep_sched);
		schedule_delayed_work(&restore_prev,
=======
		schedule_delayed_work(&c->restore_prev,
>>>>>>> 39ce106afb9a... block: Add driver to change the I/O scheduler when the screen turns off
				msecs_to_jiffies(RESTORE_DELAY_MS));
		break;
	default:
		/*
		 * Switch to noop when the screen turns off. Purposely block
		 * the fb notifier chain call in case weird things can happen
		 * when switching elevators while the screen is off.
		 */
<<<<<<< HEAD
		if (delayed_work_pending(&restore_prev))
			cancel_delayed_work_sync(&restore_prev);
		schedule_delayed_work(&sleep_sched,
				msecs_to_jiffies(RESTORE_DELAY_MS));
=======
		cancel_delayed_work_sync(&c->restore_prev);
		change_elevator(c, true);
>>>>>>> 39ce106afb9a... block: Add driver to change the I/O scheduler when the screen turns off
	}

	return NOTIFY_OK;
}

static struct notifier_block fb_notifier_callback_nb = {
	.notifier_call = fb_notifier_callback,
};

static void restore_prev_fn(struct work_struct *work)
{
<<<<<<< HEAD
	change_all_elevators(&req_queues.list, false);
}

static void set_sleep_sched_fn(struct work_struct *work)
{
	change_all_elevators(&req_queues.list, true);
=======
	struct iosched_conf *c = container_of(work, typeof(*c),
						restore_prev.work);

	change_elevator(c, false);
>>>>>>> 39ce106afb9a... block: Add driver to change the I/O scheduler when the screen turns off
}

int init_iosched_switcher(struct request_queue *q)
{
<<<<<<< HEAD
	struct req_queue_data *r;

	r = kzalloc(sizeof(*r), GFP_KERNEL);
	if (!r)
		return -ENOMEM;

	r->queue = q;

	spin_lock(&init_lock);
	list_add(&r->list, &req_queues.list);
	spin_unlock(&init_lock);

	return 0;
}

static int iosched_switcher_core_init(void)
{
	INIT_DELAYED_WORK(&restore_prev, restore_prev_fn);
	INIT_DELAYED_WORK(&sleep_sched, set_sleep_sched_fn);
=======
	struct iosched_conf *c;

	if (!q) {
		pr_err("Request queue is NULL!\n");
		return -EINVAL;
	}

	if (config_g) {
		pr_err("Already registered a request queue!\n");
		return -EINVAL;
	}

	c = kzalloc(sizeof(*c), GFP_KERNEL);
	if (!c)
		return -ENOMEM;

	c->queue = q;

	config_g = c;

	INIT_DELAYED_WORK(&c->restore_prev, restore_prev_fn);
>>>>>>> 39ce106afb9a... block: Add driver to change the I/O scheduler when the screen turns off
	fb_register_client(&fb_notifier_callback_nb);

	return 0;
}
<<<<<<< HEAD
late_initcall(iosched_switcher_core_init);
=======
>>>>>>> 39ce106afb9a... block: Add driver to change the I/O scheduler when the screen turns off
