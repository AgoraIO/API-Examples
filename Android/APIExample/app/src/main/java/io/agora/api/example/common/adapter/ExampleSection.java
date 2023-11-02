package io.agora.api.example.common.adapter;

import android.view.View;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import java.util.List;

import io.agora.api.example.MainFragment;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.github.luizgrp.sectionedrecyclerviewadapter.Section;
import io.github.luizgrp.sectionedrecyclerviewadapter.SectionParameters;

/**
 * The type Example section.
 */
public class ExampleSection extends Section {
    private final String mTitle;
    private final List<Example> mValues;
    private final MainFragment.OnListFragmentInteractionListener mListener;

    /**
     * Instantiates a new Example section.
     *
     * @param title    the title
     * @param items    the items
     * @param listener the listener
     */
    public ExampleSection(String title, List<Example> items, MainFragment.OnListFragmentInteractionListener listener) {
        super(SectionParameters.builder().headerResourceId(R.layout.layout_main_list_section).itemResourceId(R.layout.layout_main_list_item).build());
        mTitle = title;
        mValues = items;
        mListener = listener;
    }

    @Override
    public int getContentItemsTotal() {
        return mValues.size();
    }

    @Override
    public RecyclerView.ViewHolder getItemViewHolder(View view) {
        return new ViewHolder(view);
    }

    @Override
    public void onBindItemViewHolder(RecyclerView.ViewHolder viewHolder, int position) {
        if (viewHolder instanceof ViewHolder) {
            ViewHolder holder = (ViewHolder) viewHolder;
            holder.mItem = mValues.get(position);
            holder.mNameView.setText(holder.mView.getContext().getString(holder.mItem.name()));

            holder.mView.setOnClickListener(v -> {
                if (null != mListener) {
                    // Notify the active callbacks interface (the activity, if the
                    // fragment is attached to one) that an item has been selected.
                    mListener.onListFragmentInteraction(holder.mItem);
                }
            });
        }
    }

    @Override
    public RecyclerView.ViewHolder getHeaderViewHolder(View view) {
        return new ViewHolder(view);
    }

    @Override
    public void onBindHeaderViewHolder(RecyclerView.ViewHolder viewHolder) {
        if (viewHolder instanceof ViewHolder) {
            ViewHolder holder = (ViewHolder) viewHolder;
            holder.mNameView.setText(mTitle);
        }
    }

    /**
     * The type View holder.
     */
    static class ViewHolder extends RecyclerView.ViewHolder {
        /**
         * The M view.
         */
        final View mView;
        /**
         * The M name view.
         */
        final TextView mNameView;
        /**
         * The M item.
         */
        Example mItem;

        /**
         * Instantiates a new View holder.
         *
         * @param view the view
         */
        ViewHolder(View view) {
            super(view);
            mView = view;
            mNameView = view.findViewById(R.id.item_name);
        }
    }
}
